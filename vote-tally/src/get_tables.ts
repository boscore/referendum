import { delay, parseTokenString } from "./utils";
import { rpc, DELAY_MS, CONTRACT_FORUM, CONTRACT_AUDITOR } from "./config";
import { Voters, ForumVote, AuditorVotes, Proposal, Delband } from "./interfaces";

/**
 * Get Table `eosio::voters`
 */
export async function get_table_voters() {
    return (await rpc.get_all_table_rows<Voters>("eosio", "eosio", "voters", "owner")).rows;
}

/**
 * Get Table `eosio::delband`
 */
export async function get_table_delband(scopes: Set<string>) {
    const delband: Delband[] = [];
    for (const scope of Array.from(scopes)) {
        console.log(`get_table_rows [eosio::${scope}:userres]`);
        const response = await rpc.get_table_rows<Delband>("eosio", scope, "delband", {json: true });

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
    return (await rpc.get_all_table_rows<ForumVote>(CONTRACT_FORUM, CONTRACT_FORUM, "vote", "id")).rows;
}

/**
 * Get Table `auditor.bos::votes`
 */
export async function get_auditor_votes() {
    return (await rpc.get_all_table_rows<AuditorVotes>(CONTRACT_AUDITOR, CONTRACT_AUDITOR, "votes", "voter")).rows;
}

/**
 * Get Table `eosio.forum::proposal`
 */
export async function get_table_proposal() {
    return (await rpc.get_all_table_rows<Proposal>(CONTRACT_FORUM, CONTRACT_FORUM, "proposal", "proposal_name")).rows;
}
