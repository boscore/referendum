void auditorbos::nominatecand( const name cand ) {
    require_auth( cand );

    const auto candidate_itr = _candidates.find(cand.value);

    check( candidate_itr != _candidates.end(), "ERR::NOMINATECAND_NOT_FOUND::Cannot find candidate to nominate, candidate must first transfer BOS tokens to `auditor.bos` account." );
    check( !candidate_itr->is_active, "ERR::NOMINATECAND_ALREADY_REGISTERED::Candidate is already registered and active." );
    check( candidate_itr->locked_tokens >= configs().lockupasset, "ERR::NOMINATECAND_INSUFFICIENT_FUNDS_TO_STAKE::Insufficient funds have been staked." );

    // Set candidate as active
    _candidates.modify( candidate_itr, _self, [&](auto & row) {
        row.is_active = 1;
    });
}

void auditorbos::withdrawcand( const name cand ) {
    require_auth( cand );
    removeCandidate( cand, false );
}

void auditorbos::firecand( const name cand ) {
    require_auth( configs().authaccount );
    removeCandidate( cand, false );
}

void auditorbos::unstake( const name cand ) {
    require_auth( cand );

    const auto candidate_itr = _candidates.find(cand.value);

    check( candidate_itr != _candidates.end(), "ERR::UNSTAKE_CAND_NOT_REGISTERED::Candidate is not already registered." );
    check( !candidate_itr->is_active, "ERR::UNSTAKE_CANNOT_UNSTAKE_FROM_ACTIVE_CAND::Cannot unstake tokens for an active candidate. Call withdrawcand first." );
    check( candidate_itr->unstaking_end_time_stamp < time_point_sec(current_time_point()), "ERR::UNSTAKE_CANNOT_UNSTAKE_UNDER_TIME_LOCK::Cannot unstake tokens before they are unlocked from the time delay." );
    check( candidate_itr->locked_tokens.amount > 0, "ERR::UNSTAKE_ZERO_LOCKED_TOKENS::Cannot unstake tokens of zero balance." );

    _candidates.modify(candidate_itr, _self, [&](auto & row) {
        // Ensure the candidate's tokens are not locked up for a time delay period.
        // Send back the locked up tokens
        // inline transfer unstaking
        eosio::action(
                eosio::permission_level{_self , "active"_n },
                name( TOKEN_CONTRACT ),
                "transfer"_n,
                make_tuple( _self, cand, row.locked_tokens, string("Returning locked up stake. Thank you."))
        ).send();

        row.locked_tokens = asset(0, configs().lockupasset.symbol);
    });
}

void auditorbos::resign( const name auditor ) {
    require_auth( auditor );
    removeAuditor( auditor );
}

void auditorbos::fireauditor( const name auditor ) {
    require_auth( configs().authaccount );
    removeAuditor( auditor );
}

// private methods for the above actions

void auditorbos::removeAuditor( const name auditor ) {
    const auto auditor_itr = _auditors.find(auditor.value);
    check(auditor_itr != _auditors.end(), "ERR::REMOVEAUDITOR_NOT_CURRENT_AUDITOR::The entered account name is not for a current auditor.");

    // Remove auditor from the auditors table.
    _auditors.erase(auditor_itr);

    // Remove the candidate from being eligible for the next election period.
    removeCandidate(auditor, true);

    // Update the auths to give control to the new set of auditors.
    setAuditorAuths();
}

void auditorbos::removeCandidate( const name cand, const bool lockupStake ) {
    const auto candidate_itr = _candidates.find(cand.value);
    check(candidate_itr != _candidates.end(), "ERR::REMOVECANDIDATE_NOT_CURRENT_CANDIDATE::Candidate is not already registered.");

    eosio::print("Remove from nominated candidate by setting them to inactive.");

    // Set the is_active flag to false instead of deleting in order to retain votes if they return as BOS auditors.
    _candidates.modify(candidate_itr, cand, [&](auto & row) {
        row.is_active = 0;
        if (lockupStake) {
            eosio::print("Lockup stake for release delay.");
            row.unstaking_end_time_stamp = current_time_point() + time_point_sec(configs().lockup_release_time_delay);
        }
    });
}
