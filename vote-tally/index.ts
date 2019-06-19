import * as fs from "fs";
import * as path from "path";
import * as write from "write-json-file";
import * as load from "load-json-file";
import { CronJob } from "cron";
import { uploadS3 } from "./src/aws";
import { ForumVote, Proposal, Voters, Delband, AuditorVotes } from "./src/interfaces";
import { rpc, CHAIN, CONTRACT_FORUM, DEBUG, CONTRACT_TOKEN, TOKEN_SYMBOL, CONTRACT_AUDITOR } from "./src/config";
import { filterVotersByVotes, generateAccounts, generateProxies, generateTallies } from "./src/tallies";
import { get_table_voters, get_forum_vote, get_table_proposal, get_table_delband, get_auditor_votes } from "./src/get_tables";
import { disjoint, parseTokenString, createHash } from "./src/utils";
import { generateEosioStats } from "./src/stats";

// Base filepaths
const basepath = path.join(__dirname, "data", CHAIN);
const voters_latest = path.join(basepath, "eosio", "voters", "latest.json");
const delband_latest = path.join(basepath, "eosio", "delband", "latest.json");

// Global containers
let forum_votes: ForumVote[] = [];
let auditor_votes: AuditorVotes[] = [];
let voters: Voters[] = [];

// `eosioVoters` cannot be stored globaly since it will cause memory leaks
let proposals: Proposal[] = [];
let votes_owner: Set<string> = new Set();
let voters_owner: Set<string> = new Set();
let delband: Delband[] = [];
let currency_supply: any = null;

/**
 * Sync `eosio` tables
 */
async function syncEosio(head_block_num: number) {
    console.log(`syncEosio [head_block_num=${head_block_num}]`)

    // fetch `eosio` voters
    let eosioVoters: Voters[] = [];
    if (DEBUG && fs.existsSync(voters_latest)) eosioVoters = load.sync(voters_latest) // Speed up download of eosio::voters table for debugging
    else eosioVoters = await get_table_voters();

    voters = filterVotersByVotes(eosioVoters, forum_votes);
    voters_owner = new Set(voters.map((row) => row.owner));

    // Retrieve `staked` from accounts that have not yet voted for BPs
    // only `delband` from missing eosio.forum voters should be fetched
    const owners_without_stake = disjoint(votes_owner, voters_owner)
    delband = await get_table_delband(owners_without_stake);

    // Calculate EOSIO Stats
    // `eosioVoters` cannot be stored globaly since it will cause memory leaks
    console.log(`calculateEosioStats [head_block_num=${head_block_num}]`);
    const stats = generateEosioStats(head_block_num, eosioVoters);

    // Save JSON
    await save("eosio", "voters", head_block_num, eosioVoters, false);
    await save("eosio", "stats", head_block_num, stats);
    await save("referendum", "voters", head_block_num, voters);
    await save("referendum", "delband", head_block_num, delband);

    // Prevent memory leaks
    eosioVoters = [];
}

/**
 * Sync `eosio.forum` tables
 */
async function syncForum(head_block_num: number) {
    console.log(`syncForum [head_block_num=${head_block_num}]`);

    // fetch `eosio.forum` votes
    forum_votes = await get_forum_vote();
    votes_owner = new Set(forum_votes.map((row) => row.voter));

    // fetch `eosio.forum` proposal
    proposals = await get_table_proposal();

    // Save JSON
    save(CONTRACT_FORUM, "vote", head_block_num, forum_votes);
    save(CONTRACT_FORUM, "proposal", head_block_num, proposals);
}

/**
 * Sync `auditor.bos` tables
 */
async function syncAuditor(head_block_num: number) {
    console.log(`syncAuditor [head_block_num=${head_block_num}]`);

    // fetch `auditor.bos` votes
    auditor_votes = await get_auditor_votes();
    votes_owner = new Set(auditor_votes.map((row) => row.voter));

    // Save JSON
    save(CONTRACT_AUDITOR, "votes", head_block_num, auditor_votes);
}

/**
 * Sync `eosio.token` tables
 */
async function syncToken(head_block_num: number) {
    console.log(`syncToken [head_block_num=${head_block_num}]`);

    const currencyStats = await rpc.get_currency_stats(CONTRACT_TOKEN, TOKEN_SYMBOL);
    currency_supply = parseTokenString(currencyStats[TOKEN_SYMBOL].supply).amount;

    // Save JSON
    save(CONTRACT_TOKEN, "get_currency_stats", head_block_num, currencyStats);
}

/**
 * Calculate Tallies
 */
async function calculateTallies(head_block_num: number) {
    console.log(`calculateTallies [head_block_num=${head_block_num}]`);

    const accounts = generateAccounts(forum_votes, delband, voters);
    const proxies = generateProxies(forum_votes, delband, voters);
    const tallies = generateTallies(head_block_num, proposals, accounts, proxies, currency_supply);

    // Save JSON
    save("referendum", "accounts", head_block_num, accounts);
    save("referendum", "proxies", head_block_num, proxies);
    save("referendum", "tallies", head_block_num, tallies);
}

/**
 * Save JSON file
 */
async function save(account: string, table: string, block_num: number, json: any, check_exists=true) {
    const filepath = path.join(basepath, account, table, block_num + ".json");
    const latest = path.join(basepath, account, table, "latest.json");

    // Prevent saving if `latest.json` is the same as [json]
    if (check_exists) {
        if (fs.existsSync(latest)) {
            const latestJson = load.sync(latest);
            if (createHash(latestJson) === createHash(json)) {
                console.log(`JSON already exists ${account}/${table}/${block_num}.json`);
                return
            }
        }
    }

    // Save to JSON disk
    console.log(`saving JSON ${account}/${table}/${block_num}.json`);
    write.sync(filepath, json);
    write.sync(latest, json);
    await saveS3(account, table, block_num, json);
}

// Save to AWS S3 bucket
async function saveS3(account: string, table: string, block_num: number, json: any) {
    await uploadS3(`${account}/${table}/${block_num}.json`, json);
    await uploadS3(`${account}/${table}/latest.json`, json);
}

async function quickTasks() {
    const {head_block_num} = await rpc.get_info()
    await syncForum(head_block_num);
    await calculateTallies(head_block_num);
}

async function allTasks() {
    const {head_block_num} = await rpc.get_info()
    await syncAuditor(head_block_num);
    await syncToken(head_block_num);
    await syncForum(head_block_num);
    await syncEosio(head_block_num);
    await calculateTallies(head_block_num);
}

/**
 * BOS Referendum Vote Tally
 */
async function main() {
    // First initialize
    await allTasks();

    // Quick tasks (every 1 minute)
    new CronJob("* * * * *", async () => {
        await quickTasks();

    }, () => {}, true, "America/Toronto");

    // Long tasks (every 30 minutes)
    new CronJob("*/30 * * * *", async () => {
        await allTasks();

    }, () => {}, true, "America/Toronto");
}
main();

