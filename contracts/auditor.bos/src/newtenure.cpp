void auditorbos::allocate_auditors( const vector<name> candidates ) {
    contr_config conf = configs();
    check(candidates.size() == conf.numelected, "ERR::NEWTENURE_CANDIDATES_NUM_ELECTED::Number of candidates does not match the threshold defined in `numelected` configs");

    // Empty the auditors table to get a full set of new auditors.
    auto auditor_itr = _auditors.begin();
    while (auditor_itr != _auditors.end()) {
        auto candidates_itr = _candidates.find(auditor_itr->auditor_name.value);
        check( candidates_itr != _candidates.end() , "ERR::NEWTENURE_EXPECTED_CAND_NOT_FOUND::Corrupt data: Trying to set a lockup delay on candidate leaving office.");

        _candidates.modify(candidates_itr, eosio::same_payer, [&](auto & row) {
            eosio::print("Lockup stake for release delay.");
            row.unstaking_end_time_stamp = current_time_point() + time_point_sec(conf.lockup_release_time_delay);
        });
        auditor_itr = _auditors.erase(auditor_itr);
    }

    // Add appointed candidates as auditors
    for (const name candidate_name : candidates) {
        // update auditor's locked token time period
        auto candidates_itr = _candidates.find(candidate_name.value);
        check( candidates_itr != _candidates.end() , "ERR::NEWTENURE_EXPECTED_CAND_NOT_FOUND::Corrupt data: Trying to set a lockup delay on candidate leaving office.");

        // Check if candidate has the proper locked assets
        const asset locked_tokens = candidates_itr->locked_tokens;
        const asset mininum_locked_tokens = conf.lockupasset;
        check(locked_tokens.amount >= mininum_locked_tokens.amount, "ERR::NEWTENURE_CANDIDATE_REQUIRE_LOCKED_TOKENS::Candidate does not meet the minimum `locked_tokens` threshold.");
        check(candidates_itr->is_active, "ERR::NEWTENURE_CANDIDATE_ACTIVE::Candidate must be active to be elected as an auditor.");

        // Add Candidate name to auditor table
        _auditors.emplace(_self, [&](auto & row) {
            row.auditor_name = candidate_name;
        });

        // Lockup stake for release delay.
        _candidates.modify(candidates_itr, eosio::same_payer, [&](auto & row) {
            row.unstaking_end_time_stamp = current_time_point() + time_point_sec(conf.lockup_release_time_delay);
        });
    }
}

void auditorbos::set_auditor_auths() {
    name account_to_change = configs().authaccount;

    vector<eosiosystem::permission_level_weight> accounts;

    for (auto auditors_itr = _auditors.begin(); auditors_itr != _auditors.end(); auditors_itr++) {
        eosiosystem::permission_level_weight account{
                .permission = eosio::permission_level(auditors_itr->auditor_name, "active"_n),
                .weight = (uint16_t) 1,
        };
        accounts.push_back(account);
    }

    eosiosystem::authority auditors_contract_authority{
            .threshold = configs().auth_threshold_auditors,
            .keys = {},
            .accounts = accounts
    };

    action(permission_level{account_to_change, "active"_n},
           "eosio"_n, "updateauth"_n,
           std::make_tuple(
                   account_to_change,
                   AUDITORS_PERMISSION,
                   "active"_n,
                   auditors_contract_authority))
            .send();
}

void auditorbos::newtenure( const vector<name> candidates, const string message) {
    require_auth( _self );

    // Set auditors for the next period.
    allocate_auditors(candidates);

    // Set the auths on the BOS auditor authority account
    set_auditor_auths();
}
