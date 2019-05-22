import * as fs from "fs";
import * as path from "path";
import * as write from "write-json-file";
import * as load from "load-json-file";
import { CronJob } from "cron";
import { uploadS3 } from "./src/aws";
import { Vote, Proposal, Voters, Delband } from "./src/interfaces";
import { rpc, CHAIN, CONTRACT_FORUM, DEBUG, CONTRACT_TOKEN, TOKEN_SYMBOL } from "./src/config";
import { filterVotersByVotes, generateAccounts, generateProxies, generateTallies } from "./src/tallies";
import { get_table_voters, get_table_vote, get_table_proposal, get_table_delband } from "./src/get_tables";
import { disjoint, parseTokenString, createHash } from "./src/utils";

// Base filepaths
const basepath = path.join(__dirname, "data", CHAIN);
const voters_latest = path.join(basepath, "eosio", "voters", "latest.json");
const delband_latest = path.join(basepath, "eosio", "delband", "latest.json");

// Global containers
let votes: Vote[] = [];
let voters: Voters[] = [];
let proposals: Proposal[] = [];
let votes_owner: Set<string> = new Set();
let voters_owner: Set<string> = new Set();
let delband: Delband[] = [];
let currency_supply = null;

/**
 * Sync `eosio` tables
 */
async function syncEosio(head_block_num: number) {
    console.log(`syncEosio [head_block_num=${head_block_num}]`)

    // fetch `eosio` voters
    if (DEBUG && fs.existsSync(voters_latest)) voters = load.sync(voters_latest) // Speed up download process for debugging
    else voters = filterVotersByVotes(await get_table_voters(), votes);
    voters_owner = new Set(voters.map((row) => row.owner));

    // Retrieve `staked` from accounts that have not yet voted for BPs
    const owners_without_stake = disjoint(votes_owner, voters_owner)
    if (DEBUG && fs.existsSync(delband_latest)) delband = load.sync(delband_latest) // Speed up download process for debugging
    else delband = await get_table_delband(owners_without_stake);

    // Save JSON
    save("eosio", "voters", head_block_num, voters);
    save("eosio", "delband", head_block_num, delband);
}

/**
 * Sync `eosio.forum` tables
 */
async function syncForum(head_block_num: number) {
    console.log(`syncForum [head_block_num=${head_block_num}]`);

    // fetch `eosio.forum` votes
    votes = await get_table_vote();
    votes_owner = new Set(votes.map((row) => row.voter));

    // fetch `eosio.forum` proposal
    proposals = await get_table_proposal();

    // Save JSON
    save(CONTRACT_FORUM, "vote", head_block_num, votes);
    save(CONTRACT_FORUM, "proposal", head_block_num, proposals);
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

    const accounts = generateAccounts(votes, delband, voters);
    const proxies = generateProxies(votes, delband, voters);
    const tallies = generateTallies(head_block_num, proposals, accounts, proxies, currency_supply);

    // Save JSON
    save("referendum", "accounts", head_block_num, accounts);
    save("referendum", "proxies", head_block_num, proxies);
    save("referendum", "tallies", head_block_num, tallies);
}

/**
 * Save JSON file
 */
function save(account: string, table: string, block_num: number, json: any) {
    const filepath = path.join(basepath, account, table, block_num + ".json");
    const latest = path.join(basepath, account, table, "latest.json");

    // Prevent saving if `latest.json` is the same as [json]
    if (fs.existsSync(latest)) {
        const latestJson = load.sync(latest);
        if (createHash(latestJson) === createHash(json)) {
            console.log(`JSON already exists ${account}/${table}/${block_num}.json`);
            return
        }
    }

    // Save to JSON disk
    console.log(`saving JSON ${account}/${table}/${block_num}.json`);
    write.sync(filepath, json);
    write.sync(latest, json);

    // Save to AWS S3 bucket
    uploadS3(`${account}/${table}/${block_num}.json`, json);
    uploadS3(`${account}/${table}/latest.json`, json);
}

async function quickTasks() {
    const {head_block_num} = await rpc.get_info()
    await syncForum(head_block_num);
    await calculateTallies(head_block_num);
}

async function allTasks() {
    const {head_block_num} = await rpc.get_info()
    await syncToken(head_block_num);
    await syncEosio(head_block_num);
    await syncForum(head_block_num);
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

