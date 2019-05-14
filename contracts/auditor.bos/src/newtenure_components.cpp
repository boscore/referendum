void auditorbos::assertPeriodTime() {
    uint32_t timestamp = now();
    uint32_t periodBlockCount = timestamp - _currentState.lastperiodtime;
    check(periodBlockCount > configs().auditor_tenure,
                 "ERR::NEWTENURE_EARLY::New period is being called too soon. Wait until the period has completed.");
}

void auditorbos::allocateAuditors(bool early_election) {

    eosio::print("Configure auditors for the next period.");

    auditors_table auditors(_self, _self.value);
    auto byvotes = registered_candidates.get_index<"byvotesrank"_n>();
    auto cand_itr = byvotes.begin();

    int32_t electcount = configs().numelected;
    uint8_t currentAuditorCount = 0;

    if (!early_election) {
        eosio::print("Empty the auditors table to get a full set of new auditors based on the current votes.");
        auto auditor_itr = auditors.begin();
        while (auditor_itr != auditors.end()) {
            const auto &reg_candidate = registered_candidates.get(auditor_itr->auditor_name.value, "ERR::NEWTENURE_EXPECTED_CAND_NOT_FOUND::Corrupt data: Trying to set a lockup delay on candidate leaving office.");
            registered_candidates.modify(reg_candidate, auditor_itr->auditor_name, [&](candidate &c) {
                eosio::print("Lockup stake for release delay.");
                c.auditor_end_time_stamp = time_point_sec(now() + configs().lockup_release_time_delay);
            });
            auditor_itr = auditors.erase(auditor_itr);
        }
    }

    eosio::print("Select only enough candidates to fill the gaps.");
    for (auto itr = auditors.begin(); itr != auditors.end(); itr++) { ++currentAuditorCount; }

    while (currentAuditorCount < electcount) {
        if (cand_itr == byvotes.end() || cand_itr->total_votes == 0) {
            eosio::print("The pool of eligible candidates has been exhausted");
            return;
        }

        //  If the candidate is inactive or is already a auditor skip to the next one.
        if (!cand_itr->is_active || auditors.find(cand_itr->candidate_name.value) != auditors.end()) {
            cand_itr++;
        } else {
            auditors.emplace(_self, [&](auditor &c) {
                c.auditor_name = cand_itr->candidate_name;
                c.total_votes = cand_itr->total_votes;
            });

            byvotes.modify(cand_itr, cand_itr->candidate_name, [&](candidate &c) {
                    eosio::print("Lockup stake for release delay.");
                    c.auditor_end_time_stamp = time_point_sec(now() + configs().lockup_release_time_delay);
            });

            currentAuditorCount++;
            cand_itr++;
        }
    }
}

void auditorbos::setAuditorAuths() {

    auditors_table auditors(_self, _self.value);

    name accountToChange = configs().authaccount;

    vector<eosiosystem::permission_level_weight> accounts;

    for (auto it = auditors.begin(); it != auditors.end(); it++) {
        eosiosystem::permission_level_weight account{
                .permission = eosio::permission_level(it->auditor_name, "active"_n),
                .weight = (uint16_t) 1,
        };
        accounts.push_back(account);
    }

    eosiosystem::authority auditors_contract_authority{
            .threshold = configs().auth_threshold_auditors,
            .keys = {},
            .accounts = accounts
    };

    action(permission_level{accountToChange, "active"_n},
           "eosio"_n, "updateauth"_n,
           std::make_tuple(
                   accountToChange,
                   AUDITORS_PERMISSION,
                   "active"_n,
                   auditors_contract_authority))
            .send();
}

void auditorbos::newtenure(string message) {

    print("Start");

    assertPeriodTime();

    print("\nPeriodTime");

    contr_config config = configs();

    print("\nConfigs");

    // Get the max supply of the lockup asset token (eg. BOS)
    auto tokenStats = stats(name(TOKEN_CONTRACT), config.lockupasset.symbol.code().raw()).begin();

    uint64_t max_supply = tokenStats->supply.amount;

    print("\nRead stats");

    double percent_of_current_voter_engagement =
            double(_currentState.total_weight_of_votes) / double(max_supply) * 100.0;

    eosio::print("\n\nToken max supply: ", max_supply, " total votes so far: ", _currentState.total_weight_of_votes);
    eosio::print("\n\nNeed inital engagement of: ", config.initial_vote_quorum_percent, "% to start the Audit Cycle.");
    eosio::print("\n\nToken supply: ", max_supply * 0.0001, " total votes so far: ", _currentState.total_weight_of_votes * 0.0001);
    eosio::print("\n\nNeed initial engagement of: ", config.initial_vote_quorum_percent, "% to start the Audit Cycle..");
    eosio::print("\n\nNeed ongoing engagement of: ", config.vote_quorum_percent,
                 "% to allow new periods to trigger after initial activation.");
    eosio::print("\n\nPercent of current voter engagement: ", percent_of_current_voter_engagement, "\n\n");

    check(_currentState.met_initial_votes_threshold == true ||
                 percent_of_current_voter_engagement > config.initial_vote_quorum_percent,
                 "ERR::NEWTENURE_VOTER_ENGAGEMENT_LOW_ACTIVATE::Voter engagement is insufficient to activate the Audit Cycle..");
    _currentState.met_initial_votes_threshold = true;

    check(percent_of_current_voter_engagement > config.vote_quorum_percent,
                 "ERR::NEWTENURE_VOTER_ENGAGEMENT_LOW_PROCESS::Voter engagement is insufficient to process a new period");

    // Set auditors for the next period.
    allocateAuditors(false);

    // Set the auths on the BOS auditor authority account
    setAuditorAuths();

    _currentState.lastperiodtime = now();


//        Schedule the the next election cycle at the end of the period.
//        transaction nextTrans{};
//        nextTrans.actions.emplace_back(permission_level(_self,N(active)), _self, N(newtenure), std::make_tuple("", false));
//        nextTrans.delay_sec = configs().auditor_tenure;
//        nextTrans.send(N(newtenure), false);
}
