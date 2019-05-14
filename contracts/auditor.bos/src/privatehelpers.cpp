
contr_config auditorbos::configs() {
    contr_config conf = config_singleton.get_or_default(contr_config());
    config_singleton.set(conf, _self);
    return conf;
}

void auditorbos::updateVoteWeight(name auditor, int64_t weight) {
    if (weight == 0) {
        print("\n Vote has no weight - No need to continue.");
    }

    auto candItr = registered_candidates.find(auditor.value);
    if (candItr == registered_candidates.end()) {
        eosio::print("Candidate not found while updating from a transfer: ", auditor);
        return; // trying to avoid throwing errors from here since it's unrelated to a transfer action.?!?!?!?!
    }

    registered_candidates.modify(candItr, auditor, [&](auto &c) {
        c.total_votes += weight;
        eosio::print("\nchanging vote weight: ", auditor, " by ", weight);
    });
}

void auditorbos::updateVoteWeights(const vector<name> &votes, int64_t vote_weight) {
    for (const auto &auditor : votes) {
        updateVoteWeight(auditor, vote_weight);
    }

    _currentState.total_votes_on_candidates += votes.size() * vote_weight;
}

void auditorbos::modifyVoteWeights(name voter, vector<name> newVotes) {
    // This could be optimised with set diffing to avoid remove then add for unchanged votes. - later
    eosio::print("Modify vote weights: ", voter, "\n");

    uint64_t asset_name = configs().lockupasset.symbol.code().raw();
    int64_t vote_weight = 0; //ac->balance.amount;
    int64_t old_weight = 0;
    vector<name> oldVotes = {};

    //Find all cases of delegated bandwidth and sum them up
    del_bandwidth_table delband(name("eosio"), voter.value);
    auto stake = delband.begin();

    while(stake != delband.end()) {
        vote_weight += stake->net_weight.amount;
        vote_weight += stake->cpu_weight.amount;
        stake++;
    }

    // Add any liquid balance
    // accounts accountstable(name(TOKEN_CONTRACT), voter.value);
    // const auto ac = accountstable.find(asset_name);
    // if (ac != accountstable.end()) {
    //     vote_weight += ac->balance.amount;
    // }

    // Find a vote that has been cast by this voter previously.
    auto existingVote = votes_cast_by_members.find(voter.value);
    if (existingVote != votes_cast_by_members.end()) {

        old_weight = existingVote->weight; //fetch the old weight
        oldVotes = existingVote->candidates; //fetch the original candidates

        if (newVotes.size() == 0) {
            // Remove the vote if the array of candidates is empty
            votes_cast_by_members.erase(existingVote);
            eosio::print("\n Removing empty vote.");
        } else {
            votes_cast_by_members.modify(existingVote, voter, [&](vote &v) {
                v.candidates = newVotes;
                v.proxy = name();
                v.weight = vote_weight;
            });
        }
    } else {
        votes_cast_by_members.emplace(voter, [&](vote &v) {
            v.voter = voter;
            v.candidates = newVotes;
            v.weight = vote_weight;
        });
    }

    // New voter -> Add the tokens to the total weight.
    if (oldVotes.size() == 0)
        _currentState.total_weight_of_votes += vote_weight;

    // Leaving voter -> Remove the tokens to the total weight.
    if (newVotes.size() == 0)
        _currentState.total_weight_of_votes -= old_weight;

    updateVoteWeights(oldVotes, -old_weight); //remove old weights
    updateVoteWeights(newVotes, vote_weight); //add new weights
}

