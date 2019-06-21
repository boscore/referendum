import { parseTokenString } from "./utils";
import { TallyStats, EosioDelband, EosioVoter } from "./interfaces";
import { ForumVote } from "./interfaces_forum";
import { AuditorVote } from "./interfaces_auditor";

export function defaultAccount() {
    return {
        votes: {},
        staked: 0,
        proxy: "",
        is_proxy: false,
    };
}

export function defaultStats(block_num: number): TallyStats {
    return {
        votes: {
            total: 0,
            proxies: 0,
            accounts: 0,
        },
        accounts: {
            0: 0,
            1: 0,
            total: 0,
        },
        proxies: {
            0: 0,
            1: 0,
            total: 0,
        },
        staked: {
            0: 0,
            1: 0,
            total: 0,
        },
        block_num,
    };
}

export function countStaked(delband: EosioDelband) {
    if (!delband) return 0;
    const cpu = parseTokenString(delband.cpu_weight).amount;
    const net = parseTokenString(delband.net_weight).amount;
    return cpu + net;
}

export function filterVotersByVotes(voters: EosioVoter[], forum_votes: ForumVote[], auditor_votes: AuditorVote[]) {
    const results: EosioVoter[] = [];
    const voted = new Set();

    // Only track accounts who has casted votes
    for (const row of forum_votes) {
        voted.add(row.voter);
    }

    for (const row of auditor_votes) {
        voted.add(row.voter);
    }

    for (const row of voters) {
        const owner = row.owner;

        // Voter is only included if voted or proxied to a proxy who has voted
        if (voted.has(owner) || voted.has(row.proxy)) results.push(row);
    }
    return results;
}