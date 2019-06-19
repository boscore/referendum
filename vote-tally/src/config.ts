// import { JsonRpc } from "eosjs";
import { DappClient as JsonRpc } from "dapp-client";
const fetch = require("isomorphic-fetch");
require('dotenv').config()

if (!process.env.NODEOS_ENDPOINT) throw new Error("[NODEOS_ENDPOINT] is required as .env");
if (!process.env.CHAIN) throw new Error("[CHAIN] is required as .env");

// Required
export const NODEOS_ENDPOINT = process.env.NODEOS_ENDPOINT;
export const CHAIN = process.env.CHAIN;

// Optional
export const CONTRACT_FORUM = process.env.CONTRACT_FORUM || "eosio.forum";
export const CONTRACT_AUDITOR = process.env.CONTRACT_AUDITOR || "auditor.bos";
export const CONTRACT_TOKEN = process.env.CONTRACT_TOKEN || "eosio.token";
export const TOKEN_SYMBOL = process.env.TOKEN_SYMBOL || "BOS";

// Optional AWS Configs
export const AWS_BUCKET = process.env.AWS_BUCKET || "bostest.referendum";
export const AWS_ACCESS_KEY_ID = process.env.AWS_ACCESS_KEY_ID;
export const AWS_SECRET_ACCESS_KEY = process.env.AWS_SECRET_ACCESS_KEY;
export const AWS_REGION = process.env.AWS_REGION || "us-east-1";

// Debug Configs
export const DELAY_MS = Number(process.env.DELAY_MS || 10);
export const DEBUG: boolean = JSON.parse(process.env.DEBUG || "false");

// eosio RPC
export const rpc = new JsonRpc(NODEOS_ENDPOINT, {fetch})

console.log("Configurations");
console.log("--------------");
console.log("NODEOS_ENDPOINT:", NODEOS_ENDPOINT);
console.log("DELAY_MS:", DELAY_MS);
console.log("CONTRACT_FORUM:", CONTRACT_FORUM);
console.log("CONTRACT_TOKEN:", CONTRACT_TOKEN);
console.log("TOKEN_SYMBOL:", TOKEN_SYMBOL);

console.log("\nAWS Config");
console.log("----------");
console.log("AWS_BUCKET:", AWS_BUCKET);
console.log("AWS_ACCESS_KEY_ID:", AWS_ACCESS_KEY_ID);
console.log("AWS_SECRET_ACCESS_KEY:", AWS_SECRET_ACCESS_KEY);
console.log("AWS_REGION:", AWS_REGION);

console.log("\nDebug Config");
console.log("-----------");
console.log("DELAY_MS:", DELAY_MS);
console.log("DEBUG:", DEBUG + '\n');

