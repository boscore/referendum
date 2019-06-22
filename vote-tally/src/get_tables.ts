import { delay, parseTokenString } from "./utils";
import { rpc, DELAY_MS, CONTRACT_FORUM, CONTRACT_AUDITOR } from "./config";
import { EosioVoter, ForumVote, AuditorVote, ForumProposal, EosioDelband } from "./interfaces";
import { AuditorCandidate } from "./interfaces_auditor";

/**
 * Get Table `eosio::voters`
 */
export async function get_table_voters() {
    return get_tables<EosioVoter>("eosio", "eosio", "voters", "owner", ["flags1", "reserved2", "reserved3"]);
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
export function get_forum_vote() {
    return get_tables<ForumVote>(CONTRACT_FORUM, CONTRACT_FORUM, "vote", "id");
}

/**
 * Get Table `auditor.bos::votes`
 */
export function get_auditor_votes() {
    return get_tables<AuditorVote>(CONTRACT_AUDITOR, CONTRACT_AUDITOR, "votes", "voter");

}

/**
 * Get Table `eosio.forum::proposal`
 */
export function get_table_forum_proposal() {
    return get_tables<ForumProposal>(CONTRACT_FORUM, CONTRACT_FORUM, "proposal", "proposal_name");
}

/**
 * Get Table `auditor.bos::candidates`
 */
export function get_table_auditor_candidates() {
    return get_tables<AuditorCandidate>(CONTRACT_AUDITOR, CONTRACT_AUDITOR, "candidates", "candidate_name", ["total_votes"]);
}

/**
 * Get Tables
 */
export async function get_tables<T>(code: string, scope: string, table: string, lower_bound_key: string, delete_keys: string[] = []): Promise<T[]> {
    let lower_bound = "";
    const limit = 1500;
    const rows = new Map<string, T>();

    while (true) {
        console.log(`get_table_rows [${code}::${scope}:${table}] size=${rows.size} lower=${lower_bound}`);
        const response: any = await rpc.get_table_rows<T>(code, scope, table, {
            json: true,
            lower_bound,
            limit,
        });
        for (const row of response.rows) {
            // Delete extra fields
            for (const key of delete_keys) {
                delete row[key];
            }

            // Adding to Map removes duplicates entries
            const key = row[lower_bound_key];
            rows.set(key, row);

            // Set lower bound
            lower_bound = key;
        }
        // prevent hitting rate limits from API endpoints
        await delay(DELAY_MS);

        // end of table rows
        if (response.more === false) break;
    }
    return Array.from(rows.values());
}