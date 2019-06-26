void auditorbos::vote( const name voter, const vector<name> candidates, const string& vote_json ) {
    require_auth( voter );
    VALIDATE_JSON( vote_json, 32768 );

    check(candidates.size() <= configs().maxvotes, "ERR::VOTE_MAX_VOTES_EXCEEDED::Max number of allowed votes was exceeded.");
    check(candidates.size() > 0, "ERR::VOTE_MIN_VOTES_REQUIRED::Must vote for a minimum of 1 candidate.");

    std::set<name> dupSet{};
    for (name candidate_name: candidates) {
        check(dupSet.insert(candidate_name).second, "ERR::VOTE_DUPLICATE_VOTES::Added duplicate votes for the same candidate.");

        auto candidate_itr = _candidates.find(candidate_name.value);
        check(candidate_itr != _candidates.end(), "ERR::VOTE_CANDIDATE_NOT_FOUND::Candidate could not be found.");
        check(candidate_itr->is_active, "ERR::VOTE_VOTING_FOR_INACTIVE_CAND::Attempting to vote for an inactive candidate.");

        // Zero existing votes (vote counting is done offchain via BOS Referendum Tally)
        _candidates.modify(candidate_itr, eosio::same_payer, [&]( auto& row) {
            row.total_votes = 0;
        });
    }

    const auto & voter_itr = _vote.find(voter.value);

    // Modify existing registered candidate with additional locked_tokens
    if (voter_itr != _vote.end()) {
        _vote.modify(voter_itr, eosio::same_payer, [&](auto& row) {
            row.candidates = candidates;
            row.vote_json = vote_json;
            row.updated_at = current_time_point();
        });
    // New candidate, register them and add locked_tokens
    } else {
        _vote.emplace(_self, [&](auto& row) {
            row.voter = voter;
            row.candidates = candidates;
            row.vote_json = vote_json;
            row.updated_at = current_time_point();
        });
    }
}

void auditorbos::unvote( const name voter ) {
    require_auth( voter );

    const auto & vote_itr = _vote.find(voter.value);
    check(vote_itr != _vote.end(), "ERR::UNVOTE_NOT_FOUND::Cannot find an existing vote with this voter.");
    _vote.erase(vote_itr);
}
