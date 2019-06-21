import { delay, parseTokenString } from "./utils";
import { rpc, DELAY_MS, CONTRACT_FORUM, CONTRACT_AUDITOR } from "./config";
import { EosioVoter, ForumVote, AuditorVote, ForumProposal, EosioDelband } from "./interfaces";
import { AuditorCandidate } from "./interfaces_auditor";

/**
 * Get Table `eosio::voters`
 */
export async function get_table_voters() {
    const result = await rpc.get_all_table_rows<EosioVoter>("eosio", "eosio", "voters", "owner");
    return result.rows;
}

/**
 * Get Table `eosio::delband`
 */
export async function get_table_delband(scopes: Set<string>) {
    const delband: EosioDelband[] = [];
    for (const scope of Array.from(scopes)) {
        console.log(`get_table_rows [eosio::${scope}:userres]`);
        const response = await rpc.get_table_rows<EosioDelband>("eosio", scope, "delband", {json: true });

        for (const row of response.rows) {
            // Only include `delband` that is self delegated
            if (row.from == row.to) delband.push(row);
        }
    }
    return delband;
}

/**
 * Get Table `eosio.forum::vote`
 */
export async function get_forum_vote() {
    const result = await rpc.get_all_table_rows<ForumVote>(CONTRACT_FORUM, CONTRACT_FORUM, "vote", "id")
    return result.rows;
}

/**
 * Get Table `auditor.bos::votes`
 */
export async function get_auditor_votes() {
    const result = await rpc.get_all_table_rows<AuditorVote>(CONTRACT_AUDITOR, CONTRACT_AUDITOR, "votes", "voter");
    return result.rows.map(row => {
        return {
            voter: row.voter,
            candidates: row.candidates
        }
    })
}

/**
 * Get Table `eosio.forum::proposal`
 */
export async function get_table_forum_proposal() {
    const result = await rpc.get_all_table_rows<ForumProposal>(CONTRACT_FORUM, CONTRACT_FORUM, "proposal", "proposal_name");
    return result.rows;
}

/**
 * Get Table `auditor.bos::candidates`
 */
export async function get_table_auditor_candidates() {
    const result = await rpc.get_all_table_rows<AuditorCandidate>(CONTRACT_AUDITOR, CONTRACT_AUDITOR, "candidates", "candidate_name");
    return result.rows.map(row => {
        return {
            candidate_name: row.candidate_name,
            locked_tokens: row.locked_tokens,
            is_active: row.is_active,
            auditor_end_time_stamp: row.auditor_end_time_stamp,
        }
    })
}
