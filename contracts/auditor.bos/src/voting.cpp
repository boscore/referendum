void auditorbos::vote( const name voter, const vector<name> candidates, const string& vote_json ) {
    require_auth( voter );
    VALIDATE_JSON( vote_json, 32768 );

    check(candidates.size() <= configs().maxvotes, "ERR::VOTE_MAX_VOTES_EXCEEDED::Max number of allowed votes was exceeded.");
    check(candidates.size() > 0, "ERR::VOTE_MIN_VOTES_REQUIRED::Must vote for a minimum of 1 candidate.");

    std::set<name> dupSet{};
    for (name candidate_name: candidates) {
        // Check if voter has voted for duplicate canidates
        check(dupSet.insert(candidate_name).second, "ERR::VOTE_DUPLICATE_VOTES::Added duplicate votes for the same candidate.");

        // Check if candidates exists & active
        auto candidate_itr = _candidates.find(candidate_name.value);
        check(candidate_itr != _candidates.end(), "ERR::VOTE_CANDIDATE_NOT_FOUND::Candidate could not be found.");
        check(candidate_itr->is_active, "ERR::VOTE_VOTING_FOR_INACTIVE_CAND::Attempting to vote for an inactive candidate.");
    }

    // Get voter's voting information (staked & proxy)
    const auto & voter_itr = _votes.find(voter.value);
    int64_t staked = 0;
    name proxy = ""_n;
    auto candidate_itr = _voters.find(voter.value);
    if (candidate_itr != _voters.end()) {
        staked = candidate_itr->staked;
        proxy = candidate_itr->proxy;
    }

    // Modify existing registered candidate with additional locked_tokens
    if (voter_itr != _votes.end()) {
        _votes.modify(voter_itr, eosio::same_payer, [&](auto& row) {
            row.candidates = candidates;
            row.staked = staked;
            row.proxy = proxy;
        });
    // New candidate, register them and add locked_tokens
    } else {
        _votes.emplace(_self, [&](auto& row) {
            row.voter = voter;
            row.candidates = candidates;
            row.staked = staked;
            row.proxy = proxy;
        });
    }

    // Add `vote_json` to new table
    const auto & votes_json_itr = _votejson.find(voter.value);

    // Modify existing `vote_json` row
    if (votes_json_itr != _votejson.end()) {
        _votejson.modify(votes_json_itr, eosio::same_payer, [&](auto& row) {
            row.vote_json = vote_json;
            row.updated_at = current_time_point();
        });
    // Create new `vote_json row
    } else {
        _votejson.emplace(_self, [&](auto& row) {
            row.voter = voter;
            row.vote_json = vote_json;
            row.updated_at = current_time_point();
        });
    }
}

void auditorbos::unvote( const name voter ) {
    require_auth( voter );
    remove_voter( voter );
}

void auditorbos::remove_voter( const name voter ) {
    // Delete `votes` row from voter
    const auto & votes_itr = _votes.find(voter.value);
    check(votes_itr != _votes.end(), "ERR::UNVOTE_NOT_FOUND::Cannot find an existing vote with this voter.");
    _votes.erase(votes_itr);

    // Delete `votes_json` row from voter
    const auto & votejson_itr = _votejson.find(voter.value);
    if (votejson_itr != _votejson.end()) {
        _votejson.erase(votejson_itr);
    }
}
