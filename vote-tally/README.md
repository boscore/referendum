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

### `referendum` (tally)

- [referendum::forum.tallies](https://s3.amazonaws.com/bos.referendum/referendum/forum.tallies/latest.json)
- [referendum::forum.accounts](https://s3.amazonaws.com/bos.referendum/referendum/forum.accounts/latest.json)
- [referendum::forum.proxies](https://s3.amazonaws.com/bos.referendum/referendum/forum.proxies/latest.json)
- [referendum::auditor.tallies](https://s3.amazonaws.com/bos.referendum/referendum/auditor.tallies/latest.json)
- [referendum::auditor.accounts](https://s3.amazonaws.com/bos.referendum/referendum/auditor.accounts/latest.json)
- [referendum::auditor.proxies](https://s3.amazonaws.com/bos.referendum/referendum/auditor.proxies/latest.json)
- [referendum::auditor.auditors](https://s3.amazonaws.com/bos.referendum/referendum/auditor.auditors/latest.json)
- [referendum::delband](https://s3.amazonaws.com/bos.referendum/referendum/delband/latest.json)
- [referendum::voters](https://s3.amazonaws.com/bos.referendum/referendum/voters/latest.json)

## `auditor.bos`

- [auditor.bos::votes`](https://s3.amazonaws.com/bos.referendum/auditor.bos/votes/latest.json)
- [auditor.bos::votejson`](https://s3.amazonaws.com/bos.referendum/auditor.bos/votejson/latest.json)
- [auditor.bos::candidates`](https://s3.amazonaws.com/bos.referendum/auditor.bos/candidates/latest.json)
- [auditor.bos::auditors`](https://s3.amazonaws.com/bos.referendum/auditor.bos/auditors/latest.json)
- [auditor.bos::config`](https://s3.amazonaws.com/bos.referendum/auditor.bos/config/latest.json)
- [auditor.bos::bios`](https://s3.amazonaws.com/bos.referendum/auditor.bos/bios/latest.json)

## `eosio.forum`

- [eosio.forum::vote`](https://s3.amazonaws.com/bos.referendum/eosio.forum/vote/latest.json)
- [eosio.forum::proposal`](https://s3.amazonaws.com/bos.referendum/eosio.forum/proposal/latest.json)

### `eosio`

- [eosio::voters`](https://s3.amazonaws.com/bos.referendum/eosio/voters/latest.json)
- [eosio::stats`](https://s3.amazonaws.com/bos.referendum/eosio/stats/latest.json)