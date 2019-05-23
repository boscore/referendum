# BOS Referendum - Vote Tally

> Aggregates proposals/votes/voter staked weights from `eosio.forum`.

## Install

```bash
git clone https://github.com/boscore/referendum.git
cd referendum
git checkout feature/vote-tally
cd vote-tally
npm install
```

## Quick Start

```bash
npm start
```

## ENV Variables

```env
# Configuration
CHAIN="bos"
NODEOS_ENDPOINT="https://bos.eosn.io"
CONTRACT_FORUM="eosio.forum"
CONTRACT_TOKEN="eosio.token"
TOKEN_SYMBOL="BOS"

# AWS Config
AWS_BUCKET="bos.referendum"
AWS_ACCESS_KEY_ID="<ACCESS KEY>"
AWS_SECRET_ACCESS_KEY="<SECRET KEY>"
AWS_REGION="us-east-1"
```

## Using `eosc forum`

**vote**

```bash
bosc forum vote [voter] [proposal_name] [vote_value]
```

**proposal**

```bash
bosc forum propose [proposer] [proposal_name] [title] [proposal_expiration_date]
```

## S3 Bucket URL template

- [https://s3.amazonaws.com/bos.referendum/{scope}/{table}/{block_num}.json](https://s3.amazonaws.com/bos.referendum/referendum/tallies/latest.json)

#### `referendum` (tally)

`referendum::tallies` (tallies for `eosio.forum` voters)

- https://s3.amazonaws.com/bos.referendum/referendum/tallies/latest.json

`referendum::accounts` (account details for `eosio.forum` voters)

- https://s3.amazonaws.com/bos.referendum/referendum/accounts/latest.json

`referendum::proxies` (proxies details for `eosio.forum` voters)

- https://s3.amazonaws.com/bos.referendum/referendum/proxies/latest.json

`referendum::delband` (self delegated bandwidth amount for all `eosio.forum` voters)

- https://s3.amazonaws.com/bos.referendum/referendum/delband/latest.json

`referendum::voters` (voters table for all `eosio.forum` voters)

- https://s3.amazonaws.com/bos.referendum/referendum/voters/latest.json

`referendum::summaries` (aggregated statistics useful for UI's)

- https://s3.amazonaws.com/bos.referendum/referendum/summaries/latest.json

#### `eosio.forum` (eosio.forum)

`eosio.forum::vote` (all votes)

- https://s3.amazonaws.com/bos.referendum/eosio.forum/vote/latest.json

`eosio.forum::proposal` (all proposals)

- https://s3.amazonaws.com/bos.referendum/eosio.forum/proposal/latest.json

#### `eosio` (voting weights)

`eosio::voters` (entire voters table)

- https://s3.amazonaws.com/bos.referendum/eosio/voters/latest.json
