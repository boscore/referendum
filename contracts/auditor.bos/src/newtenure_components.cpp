void auditorbos::allocateAuditors(vector<name> candidates) {
    auditors_table auditors(_self, _self.value);

    // Empty the auditors table to get a full set of new auditors.
    auto auditor_itr = auditors.begin();
    while (auditor_itr != auditors.end()) {
        const auto &reg_candidate = registered_candidates.get(auditor_itr->auditor_name.value, "ERR::NEWTENURE_EXPECTED_CAND_NOT_FOUND::Corrupt data: Trying to set a lockup delay on candidate leaving office.");
        registered_candidates.modify(reg_candidate, auditor_itr->auditor_name, [&](candidate & row) {
            eosio::print("Lockup stake for release delay.");
            row.auditor_end_time_stamp = time_point_sec(now() + configs().lockup_release_time_delay);
        });
        auditor_itr = auditors.erase(auditor_itr);
    }

    // Add appointed candidates as auditors
    auto cand_itr = candidates.begin();
    for (const name candidate_name : candidates){
        // Candidate name to auditor table
        auditors.emplace(_self, [&](auditor & row) {
            row.auditor_name = candidate_name;
        });
        // update auditor's locked token time period
        const auto &reg_candidate = registered_candidates.get(candidate_name.value, "ERR::NEWTENURE_EXPECTED_CAND_NOT_FOUND::Corrupt data: Trying to set a lockup delay on candidate leaving office.");
        registered_candidates.modify(reg_candidate, candidate_name, [&](candidate & row) {
            eosio::print("Lockup stake for release delay.");
            row.auditor_end_time_stamp = time_point_sec(now() + configs().lockup_release_time_delay);
        });
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

void auditorbos::newtenure(vector<name> candidates, string message) {

    // Set auditors for the next period.
    allocateAuditors(candidates);

    // Set the auths on the BOS auditor authority account
    setAuditorAuths();

    _currentState.lastperiodtime = now();
}
