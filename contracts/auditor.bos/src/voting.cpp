
void auditorbos::voteauditor(name voter, vector<name> newvotes) {
#ifdef VOTING_DISABLED
    check(false,"ERR::VOTEAUDITOR_VOTING_IS_DISABLED::Voting is currently disabled.");
#endif

    require_auth(voter);

    check(newvotes.size() <= configs().maxvotes, "ERR::VOTEAUDITOR_MAX_VOTES_EXCEEDED::Max number of allowed votes was exceeded.");
    std::set<name> dupSet{};
    for (name vote: newvotes) {
        check(dupSet.insert(vote).second, "ERR::VOTEAUDITOR_DUPLICATE_VOTES::Added duplicate votes for the same candidate.");
        auto candidate = registered_candidates.get(vote.value, "ERR::VOTEAUDITOR_CANDIDATE_NOT_FOUND::Candidate could not be found.");
        check(candidate.is_active, "ERR::VOTEAUDITOR_VOTING_FOR_INACTIVE_CAND::Attempting to vote for an inactive candidate.");
    }

    modifyVoteWeights(voter, newvotes);
}
