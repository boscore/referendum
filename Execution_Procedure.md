## Example

Proposer：`pursonpurson`

Auditor：`directflight`

bet: betbetbet111 - betbetbet121

https://bos-pull.keosd.io/#/poll_detail?proposal=newtest

## Command Line Quick Start

We use the eosc cmd tool blow. [https://bosc.app/](https://eosc.app/)

alias bosc= "eosc -u https://xxxxxxxxxxxx"

## Part 1. Proposer And User 

![img](https://uploader.shimo.im/f/K0qO5RiIfVoFNxbU.png!thumbnail)       

Detail: https://github.com/EOS-Nation/eosio.forum/blob/master/README.md

```
--popose (for proposer)
$ bosc tx create eosio.forum propose '{"proposer": "proposer1", "proposal_name": "example", "title": "The title, for list views", "proposal_json": "", "expires_at": "2019-01-30T17:03:20"}' -p proposer1@active


--vote (for user)
$ bosc tx create eosio.forum vote '{"voter": "voter1", "proposal_name": "example", "vote": 1, "vote_json": ""}' -p voter1@active
```

## Part 2. Auditor And User 

Detail: https://github.com/EOS-Nation/auditor.bos/blob/master/README.md

```
--stake asset (for auditor candidate)
$ bosc transfer <CANDIDATE> auditor.bos "100.0000 BOS" -m "stake for auditor.bos"


--nominate to a candidate (for auditor candidate)
$ bosc tx create auditor.bos nominatecand '{"cand": "<CANDIDATE>"}' -p <CANDIDATE>@active
```

### Vote for Auditor Candidate

```
--vote (for user)
$ bosc tx create auditor.bos voteauditor '{"voter":"<VOTER>","newvotes":["<CANDIDATE_1>", "<CANDIDATE_2>","<CANDIDATE_3>"]}' -p deniscarrier
```



## Part 3. BET and BPs 

Detial:https://github.com/EOS-Nation/escrow.bos/blob/master/README.md

![img](https://uploader.shimo.im/f/030svcQ4BmEB84nz.png!thumbnail)       



### Proposal Approved

```
--init (for BET)
$ bosc tx create escrow.bos init '{"sender":"bet.bos","receiver":"<RECEIVER>","approver":"eosio","escrow_name":"<NAME>","expires_at":"2019-09-15T00:00:00","memo":"BOS escrow"}' -p bet.bos


--Fund/Initialize Escrow (for BET)
$ bosc transfer bet.bos escrow.bos "100.0000 BOS" -m "Fund BOS escrow" -p bet.bos




-- Approve MSIG (for BET)
$ bosc tx create escrow.bos approve '{"escrow_name": "<NAME>", "approver":"bet.bos"}' -p bet.bos --skip-sign --expiration 36000 --write-transaction approve.json




-- Propose MSIG (for BET)
$ bosc multisig propose <PROPOSER> <PROPOSAL NAME> approve.json --request-producers




-- BP approve (for BET)
$ bosc tx create escrow.bos approve '{"escrow_name":"<NAME>","approver":"bet.bos"}' -p <BET ACCOUNT>




-- Exec MSIG (for BET)
$ bosc multisig exec <PROPOSER> <PROSOAL NAME> <EXECUTER>




--Claim Escrow 100% (for proposer)
$ bosc tx create escrow.bos claim '{"escrow_name":"<NAME>"}' -p
```

### Proposal Dispproved by BET and Approved by BPs

```
--init (for BET)
$ bosc tx create escrow.bos init '{"sender":"bet.bos","receiver":"<RECEIVER>","approver":"eosio","escrow_name":"<NAME>","expires_at":"2019-09-15T00:00:00","memo":"BOS escrow"}' -p bet.bos




--Fund/Initialize Escrow (for BET)
$ bosc transfer bet.bos escrow.bos "100.0000 BOS" -m "Fund BOS escrow" -p bet.bos


-- Approve MSIG (for BPs)
$ bosc tx create escrow.bos approve '{"escrow_name": "<NAME>", "approver":"eosio"}' -p eosio --skip-sign --expiration 36000 --write-transaction approve.json


-- Propose MSIG (for BPs)
$ bosc multisig propose <PROPOSER> <PROPOSAL NAME> approve.json --request-producers


-- BP approve (for BPs)
$ bosc tx create escrow.bos approve '{"escrow_name":"<NAME>","approver":"eosio"}' -p <BP ACCOUNT>


-- Exec MSIG (for BPs)
$ bosc multisig exec <PROPOSER> <PROSOAL NAME> <EXECUTER>


--90% Claim Escrow  (for proposer)
$ bosc tx create escrow.bos claim '{"escrow_name":"<NAME>"}' -p <ACCOUNT>


--10% Transfer to each BPs and auditors manually  (for BET)


$ bosc tx create eosio.token transfer '{"from": "bet.bos", "to":<ACCOUNT>,"quantity":"0.9000 BOS","memo":"Thanks for your opinions"}' -p bet.bos --skip-sign --expiration 36000 --write-transaction approve.json


-- Propose MSIG (for BET)
$ bosc multisig propose <PROPOSER> <PROPOSAL NAME> approve.json --request <BET ACCOUNT>CC,<BET ACCOUNT>...(7 accounts)


-- MSIG approve (for BET)
$ bosc multisig approve <PROPOSER> <PROPOSAL NAME> <APPROVER> 


-- Exec MSIG (for BET)
$ bosc multisig exec <PROPOSER> <PROSOAL NAME> 
```
