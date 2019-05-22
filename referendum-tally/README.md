# BOS Referendum Tally

> Aggregates proposals/votes/voter staked weights from `eosio.forum`.

## Install

```bash
git clone https://github.com/EOS-Nation/bos-referendum-tally.git
cd bos-referendum-tally
npm install
```

## Quick Start

```bash
npm start
```

## ENV Variables

```env
# Configuration
CHAIN="bostest"
NODEOS_ENDPOINT="https://bostest.eosn.io"
CONTRACT_FORUM="eosio.forum"
CONTRACT_TOKEN="eosio.token"
TOKEN_SYMBOL="BOS"

# AWS Config
AWS_BUCKET="bostest.referendum"
AWS_ACCESS_KEY_ID="<ACCESS KEY>"
AWS_SECRET_ACCESS_KEY="<SECRET KEY>"
AWS_REGION="us-east-1"

# Debug Config
DELAY_MS=10
DEBUG=false
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

- [https://s3.amazonaws.com/bostest.referendum/{scope}/{table}/{block_num}.json](https://s3.amazonaws.com/bostest.referendum/referendum/tallies/latest.json)

#### `referendum` (tally)

`referendum::tallies` (tallies for `eosio.forum` voters)

- https://s3.amazonaws.com/bostest.referendum/referendum/tallies/latest.json

`referendum::accounts` (account details for `eosio.forum` voters)

- https://s3.amazonaws.com/bostest.referendum/referendum/accounts/latest.json

`referendum::proxies` (proxies details for `eosio.forum` voters)

- https://s3.amazonaws.com/bostest.referendum/referendum/proxies/latest.json

#### `eosio.forum` (eosio.forum)

`eosio.forum::vote` (all votes)

- https://s3.amazonaws.com/bostest.referendum/eosio.forum/vote/latest.json

`eosio.forum::proposal` (all proposals)

- https://s3.amazonaws.com/bostest.referendum/eosio.forum/proposal/latest.json

#### `eosio` (voting weights)

`eosio::voters` (complete voters table)

- https://s3.amazonaws.com/bostest.referendum/eosio/voters/latest.json

`eosio::delband` (self delegated bandwidth amount for all `eosio.forum` voters)

- https://s3.amazonaws.com/bostest.referendum/eosio/delband/latest.json
