
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


void auditorbos::refreshvote(name voter) {
    // Anyone can refresh vote
    // background processes will typically execute this action

    // if (configs().authaccount == name{0}) {
    //     require_auth(_self);
    // } else {
    //     require_auth(configs().authaccount);
    // }

    // Find a vote that has been cast by this voter previously.
    auto existingVote = votes_cast_by_members.find(voter.value);
    if (existingVote != votes_cast_by_members.end()) {
        //new votes is same as old votes, will just apply the deltas
        modifyVoteWeights(voter, existingVote->candidates);
    }
}