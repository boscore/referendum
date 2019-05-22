import { Voters, Summaries } from "./interfaces";

export function generateSummaries(head_block_num: number, voters: Voters[]): Summaries {
    let bp_votes = 0;

    // Accumulate staked BOS for voters that have voted for BP's
    for (const voter of voters) {
        // Voter must vote for BP or proxy
        if (!voter.producers.length && !voter.proxy) continue;
        bp_votes += Number(voter.staked);
    }

    return {
        block_num: head_block_num,
        bp_votes
    }
}
