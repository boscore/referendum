import { EosioVoter, EosioStats } from "./interfaces";

export function generateEosioStats(head_block_num: number, voters: EosioVoter[], currency_supply: number): EosioStats {
    let bp_votes = 0;
    let bp_producers_votes = 0;
    let bp_proxy_votes = 0;

    // Accumulate staked BOS for voters that have voted for BP's
    for (const voter of voters) {
        const staked = Number(voter.staked);

        // Voter must vote for BP or proxy
        if (!voter.producers.length && !voter.proxy) continue;

        // Total BP Votes
        bp_votes += staked;

        // Voters voting for BP's
        if (voter.producers.length) bp_producers_votes += staked;

        // Proxies voting for BP's
        if (voter.proxy) bp_proxy_votes += staked;
    }

    return {
        block_num: head_block_num,
        bp_votes,
        bp_producers_votes,
        bp_proxy_votes,
        currency_supply,
    }
}
