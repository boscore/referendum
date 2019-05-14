
void auditorbos::transfer(name from,
                            name to,
                            asset quantity,
                            string memo) {
    if (to == _self) {
        pendingstake_table_t pendingstake(_self, _self.value);
        auto source = pendingstake.find(from.value);
        if (source != pendingstake.end()) {
            pendingstake.modify(source, _self, [&](tempstake &s) {
                s.quantity += quantity;
            });
        } else {
            pendingstake.emplace(_self, [&](tempstake &s) {
                s.sender = from;
                s.quantity = quantity;
                s.memo = memo;
            });
        }
    }

    //TODO: We can't listen to all EOSIO transfers without system contract changes
    //users will have to manually update their votes
    //we can clone this functionality in an updatevote function

    // eosio::print("\n > transfer from : ", from, " to: ", to, " quantity: ", quantity);

    // if (quantity.symbol == configs().lockupasset.symbol) {
    //     // Update vote weight for the 'from' in the transfer if vote exists
    //     auto existingVote = votes_cast_by_members.find(from.value);
    //     if (existingVote != votes_cast_by_members.end()) {
    //         updateVoteWeights(existingVote->candidates, -quantity.amount);
    //         _currentState.total_weight_of_votes -= quantity.amount;
    //     }

    //     // Update vote weight for the 'to' in the transfer if vote exists
    //     existingVote = votes_cast_by_members.find(to.value);
    //     if (existingVote != votes_cast_by_members.end()) {
    //         updateVoteWeights(existingVote->candidates, quantity.amount);
    //         _currentState.total_weight_of_votes += quantity.amount;
    //     }
    // }
}
