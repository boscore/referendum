/**
 * ### ACTION `refreshcand`
 *
 * > Used to refresh `candidate`
 *
 * > Authorized by `require_auth( _self )`
 *
 * - set `total_votes` to 0
 * - set `is_active` if locked_tockens met minimum threshold
 */
void auditorbos::refreshcand( const name cand ) {
    require_auth( _self );

    const auto & candidates_itr = _candidates.find(cand.value);
    check(candidates_itr != _candidates.end(), "ERR::REFRESH_CAND_NOT_FOUND::Cannot find an existing candidate.");

    bool is_active = false;
    if (candidates_itr->locked_tokens >= configs().lockupasset) is_active = true;

    _candidates.modify(candidates_itr, eosio::same_payer, [&](auto& row) {
        // Zero existing votes (vote counting is done offchain via BOS Referendum Tally)
        row.total_votes = 0;
        row.is_active = is_active;
    });
}

/**
 * ### ACTION `refreshvoter`
 *
 * > Used to refresh `voter`
 *
 * > Authorized by `require_auth( _self )`
 *
 * - If voter has not voted for any candidates, remove voter from `votes` & `votejson`
 * - Update voter's staked & proxy data
 * - Add `vote_json` if not present in `votejson` table
 */
void auditorbos::refreshvoter( const name voter ) {
    require_auth( _self );

    const auto & votes_itr = _votes.find(voter.value);
    check(votes_itr != _votes.end(), "ERR::REFRESH_VOTER_NOT_FOUND::Cannot find an existing voter.");

    // If voter has not voted for any candidates, remove voter from `votes` & `votejson`
    if (votes_itr->candidates.size() == 0) {
        remove_voter( voter );
        return;
    }

    // Get voter's voting information (staked & proxy)
    int64_t staked = 0;
    name proxy = ""_n;
    auto voters_itr = _voters.find(voter.value);
    if (voters_itr != _voters.end()) {
        // Update staked & proxy details from voter
        _votes.modify(votes_itr, eosio::same_payer, [&](auto& row) {
            staked = voters_itr->staked;
            proxy = voters_itr->proxy;
        });
    }

    // Add `votejson` row if not present
    const auto & votejson_itr = _votejson.find(voter.value);
    if (votejson_itr == _votejson.end()) {
        _votejson.emplace(_self, [&](auto& row) {
            row.voter = voter;
            row.vote_json = "";
            row.updated_at = current_time_point();
        });
    }
}