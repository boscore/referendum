void auditorbos::vote(name voter, vector<name> newvotes) {
    require_auth( voter );

    check(newvotes.size() <= configs().maxvotes, "ERR::VOTE_MAX_VOTES_EXCEEDED::Max number of allowed votes was exceeded.");
    check(newvotes.size() > 0, "ERR::VOTE_MIN_VOTES_REQUIRED::Must vote for a minimum of 1 candidate.");

    std::set<name> dupSet{};
    for (name vote: newvotes) {
        check(dupSet.insert(vote).second, "ERR::VOTE_DUPLICATE_VOTES::Added duplicate votes for the same candidate.");
        auto candidate = registered_candidates.get(vote.value, "ERR::VOTE_CANDIDATE_NOT_FOUND::Candidate could not be found.");
        check(!candidate.is_active, "ERR::VOTE_VOTING_FOR_INACTIVE_CAND::Attempting to vote for an inactive candidate.");
    }

    const auto & vote_itr = votes_cast_by_members.find(voter.value);

    // Modify existing registered candidate with additional locked_tokens
    if (vote_itr != votes_cast_by_members.end()) {
        votes_cast_by_members.modify(vote_itr, eosio::same_payer, [&](auto & row) {
            row.candidates = newvotes;
        });
    // New candidate, register them and add locked_tokens
    } else {
        votes_cast_by_members.emplace(_self, [&](auto & row) {
            row.voter = voter;
            row.candidates = newvotes;
        });
    }
}

void auditorbos::unvote(name voter) {
    require_auth( voter );

    const auto & vote_itr = votes_cast_by_members.find(voter.value);
    check(vote_itr != votes_cast_by_members.end(), "ERR::UNVOTE_NOT_FOUND::Cannot find an existing vote with this voter.");
    votes_cast_by_members.erase(vote_itr);
}
