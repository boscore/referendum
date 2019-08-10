import * as fs from "fs";
import * as path from "path";
import * as write from "write-json-file";
import * as load from "load-json-file";
import { CronJob } from "cron";
import { uploadS3 } from "./src/aws";
import { ForumVote, ForumProposal, EosioVoter, EosioDelband, AuditorVote, AuditorCandidate, AuditorAuditors, AuditorVoteJSON, AuditorBios, AuditorConfig } from "./src/interfaces";
import { rpc, CHAIN, CONTRACT_FORUM, DEBUG, CONTRACT_TOKEN, TOKEN_SYMBOL, CONTRACT_AUDITOR } from "./src/config";
import { generateForumAccounts, generateForumProxies, generateForumTallies } from "./src/tallies_forum";
import { filterVotersByVotes } from "./src/tallies";
import { get_table_delband, get_tables } from "./src/get_tables";
import { disjoint, parseTokenString, createHash } from "./src/utils";
import { generateEosioStats } from "./src/stats";
import { generateAuditorAccounts, generateAuditorTallies, generateAuditorProxies, combineAuditorVotes } from "./src/tallies_auditor";

// Base filepaths
const basepath = path.join(__dirname, "data", CHAIN);
const voters_latest = path.join(basepath, "eosio", "voters", "latest.json");

// Global Eosio
let currency_supply: any = null;

// Global Referendum
let voters: EosioVoter[] = [];
let delband: EosioDelband[] = [];
let voters_owner: Set<string> = new Set();
let votes_owner: Set<string> = new Set();

// Global Forum
let forum_votes: ForumVote[] = [];
let forum_proposals: ForumProposal[] = [];

// Global Auditor
let auditor_votes: AuditorVote[] = [];
let auditor_candidates: AuditorCandidate[] = [];
let auditor_auditors: AuditorAuditors[] = [];
let auditor_votejson: AuditorVoteJSON[] = [];
let auditor_bios: AuditorBios[] = [];
let auditor_config: AuditorConfig;

/**
 * Sync `eosio` tables
 */
async function syncEosio(head_block_num: number) {
    console.log(`syncEosio [head_block_num=${head_block_num}]`)

    // fetch `eosio` voters
    let eosioVoters: EosioVoter[] = [];
    if (DEBUG && fs.existsSync(voters_latest)) eosioVoters = load.sync(voters_latest) // Speed up download of eosio::voters table for debugging
    else eosioVoters = await get_tables<EosioVoter>("eosio", "eosio", "voters", "owner", ["flags1", "reserved2", "reserved3"]);

    voters = filterVotersByVotes(eosioVoters, forum_votes, auditor_votes);
    voters_owner = new Set(voters.map((row) => row.owner));

    // Retrieve `staked` from accounts that have not yet voted for BPs
    // only `delband` from missing eosio.forum voters should be fetched
    const owners_without_stake = disjoint(votes_owner, voters_owner)
    delband = await get_table_delband(owners_without_stake);

    // Calculate EOSIO Stats
    // `eosioVoters` cannot be stored globaly since it will cause memory leaks
    console.log(`calculateEosioStats [head_block_num=${head_block_num}]`);
    const stats = generateEosioStats(head_block_num, eosioVoters, currency_supply);

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
    forum_votes = await get_tables<ForumVote>(CONTRACT_FORUM, CONTRACT_FORUM, "vote", "id");

    // Add unique voters to global tracking
    for (const {voter} of forum_votes) {
        votes_owner.add(voter);
    }

    // fetch `eosio.forum` proposal
    forum_proposals = await get_tables<ForumProposal>(CONTRACT_FORUM, CONTRACT_FORUM, "proposal", "proposal_name");

    // Save JSON
    save(CONTRACT_FORUM, "vote", head_block_num, forum_votes);
    save(CONTRACT_FORUM, "proposal", head_block_num, forum_proposals);
}

/**
 * Sync `auditor.bos` tables
 */
async function syncAuditor(head_block_num: number) {
    console.log(`syncAuditor [head_block_num=${head_block_num}]`);

    // fetch `auditor.bos` votes
    auditor_votes = await get_tables<AuditorVote>(CONTRACT_AUDITOR, CONTRACT_AUDITOR, "votes", "voter");
    auditor_votejson = await get_tables<AuditorVoteJSON>(CONTRACT_AUDITOR, CONTRACT_AUDITOR, "votejson", "voter");
    auditor_candidates = await get_tables<AuditorCandidate>(CONTRACT_AUDITOR, CONTRACT_AUDITOR, "candidates", "candidate_name");
    auditor_auditors = await get_tables<AuditorAuditors>(CONTRACT_AUDITOR, CONTRACT_AUDITOR, "auditors", "auditor_name");
    auditor_bios = await get_tables<AuditorBios>(CONTRACT_AUDITOR, CONTRACT_AUDITOR, "bios", "candidate_name");
    auditor_config = (await get_tables<AuditorConfig>(CONTRACT_AUDITOR, CONTRACT_AUDITOR, "config", "lockupasset"))[0];

    // Add unique voters to global tracking
    for (const {voter} of auditor_votes) {
        votes_owner.add(voter);
    }

    // Save JSON
    save(CONTRACT_AUDITOR, "votes", head_block_num, auditor_votes);
    save(CONTRACT_AUDITOR, "votejson", head_block_num, auditor_votejson);
    save(CONTRACT_AUDITOR, "candidates", head_block_num, auditor_candidates);
    save(CONTRACT_AUDITOR, "auditors", head_block_num, auditor_auditors);
    save(CONTRACT_AUDITOR, "bios", head_block_num, auditor_bios);
    save(CONTRACT_AUDITOR, "config", head_block_num, auditor_config);
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
 * Calculate Forum Tallies
 */
async function calculateForumTallies(head_block_num: number) {
    console.log(`calculateForumTallies [head_block_num=${head_block_num}]`);

    const forum_accounts = generateForumAccounts(forum_votes, delband, voters);
    const forum_proxies = generateForumProxies(forum_votes, delband, voters);
    const forum_tallies = generateForumTallies(head_block_num, forum_proposals, forum_accounts, forum_proxies, currency_supply);

    // Save JSON
    save("referendum", "forum.accounts", head_block_num, forum_accounts);
    save("referendum", "forum.proxies", head_block_num, forum_proxies);
    save("referendum", "forum.tallies", head_block_num, forum_tallies);
}

/**
 * Calculate Auditor Tallies
 */
async function calculateAuditorTallies(head_block_num: number) {
    console.log(`calculateAuditorTallies [head_block_num=${head_block_num}]`);

    const auditor_accounts = generateAuditorAccounts(auditor_votes, delband, voters);
    const auditor_proxies = generateAuditorProxies(auditor_votes, delband, voters);
    const auditor_tallies = generateAuditorTallies(head_block_num, auditor_candidates, auditor_accounts, auditor_proxies, auditor_bios, auditor_auditors);
    const auditor_combined_votes = combineAuditorVotes(auditor_votes, auditor_votejson);

    // Save JSON
    save("referendum", "auditor.accounts", head_block_num, auditor_accounts);
    save("referendum", "auditor.proxies", head_block_num, auditor_proxies);
    save("referendum", "auditor.tallies", head_block_num, auditor_tallies);
    save("referendum", "auditor.votes", head_block_num, auditor_combined_votes);
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
    if (!DEBUG) {
        await uploadS3(`${account}/${table}/${block_num}.json`, json);
        await uploadS3(`${account}/${table}/latest.json`, json);
    }
}

async function quickTasks() {
    const {head_block_num} = await rpc.get_info()
    await syncForum(head_block_num);
    await syncAuditor(head_block_num);
    await calculateForumTallies(head_block_num);
    await calculateAuditorTallies(head_block_num);
}

async function allTasks() {
    const {head_block_num} = await rpc.get_info()
    await syncAuditor(head_block_num);
    await syncToken(head_block_num);
    await syncForum(head_block_num);
    await syncEosio(head_block_num);
    await calculateForumTallies(head_block_num);
    await calculateAuditorTallies(head_block_num);
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

