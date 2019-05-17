# Referndum Execution Procedure


## Command Line Quick Start

We use the eosc cmd tool blow. [https://eosc.app/](https://eosc.app/)

alias bosc= "eosc -u https://xxxxxxxxxxxx"

## Part 1. Proposal Submission and Referendum

![img](https://uploader.shimo.im/f/K0qO5RiIfVoFNxbU.png!thumbnail)       

Detail: https://github.com/boscore/referendum/tree/master/contracts/eosio.forum


Proposer proposes proposal
```
$ bosc tx create eosio.forum propose '{"proposer": "proposer1", "proposal_name": "example", "title": "The title, for list views", "proposal_json": "", "expires_at": "2019-01-30T17:03:20"}' -p proposer1@active
```

Voter votes proposal
```
bosc tx create eosio.forum vote '{"voter": "voter1", "proposal_name": "example", "vote": 1, "vote_json": ""}' -p voter1@active
```

## Part 2. Auditor Nomination and Elections

Detail: https://github.com/boscore/referendum/tree/master/contracts/auditor.bos


Candidate stakes token
```
bosc transfer <CANDIDATE> auditor.bos "100.0000 BOS" -m "stake for auditor.bos"
```

Candidate nominates
```
bosc tx create auditor.bos nominatecand '{"cand": "<CANDIDATE>"}' -p <CANDIDATE>@active
```

### Vote for Auditor Candidate


Voter votes for Auditor Candidate
```
bosc tx create auditor.bos voteauditor '{"voter":"<VOTER>","newvotes":["<CANDIDATE_1>", "<CANDIDATE_2>","<CANDIDATE_3>"]}' -p deniscarrier
```



## Part 3. BET and BPs 

Detail: https://github.com/boscore/referendum/tree/master/contracts/escrow.bos

![img](https://uploader.shimo.im/f/0YbGxhOpqG4U5ObT.png)       



### Proposal Approved


BET Initialize escrow
```
bosc tx create escrow.bos init '{"sender":"bet.bos","receiver":"<RECEIVER>","approver":"eosio","escrow_name":"<NAME>","expires_at":"2019-09-15T00:00:00","memo":"BOS escrow"}' -p bet.bos
```

BET transfer Fund
```
bosc transfer bet.bos escrow.bos "100.0000 BOS" -m "Fund BOS escrow" -p bet.bos
```



BET approve escrow (MSIG)
```
bosc tx create escrow.bos approve '{"escrow_name": "<NAME>", "approver":"bet.bos"}' -p bet.bos --skip-sign --expiration 36000 --write-transaction approve.json
```


Add Top 30 Block Producers to approval list 
```
bosc multisig propose <PROPOSER> <PROPOSAL NAME> approve.json --request-producers
```


BP approve the escrow
```
bosc tx create escrow.bos approve '{"escrow_name":"<NAME>","approver":"eosio"}' -p <BP ACCOUNT>
```

Execute MSIG (by anyone)
```
bosc multisig exec <PROPOSER> <PROSOAL NAME> <EXECUTER> -p <EXECUTER>
```


Claim Escrow 100% (by anyone)
```
bosc tx create escrow.bos claim '{"escrow_name":"<NAME>"}' -p <CLAIMER>
```

### Proposal Dispproved by BET and Approved by BPs

--init (for BET)
```
bosc tx create escrow.bos init '{"sender":"bet.bos","receiver":"<RECEIVER>","approver":"eosio","escrow_name":"<NAME>","expires_at":"2019-09-15T00:00:00","memo":"BOS escrow"}' -p bet.bos
```



--Fund/Initialize Escrow (for BET)
```
bosc transfer bet.bos escrow.bos "100.0000 BOS" -m "Fund BOS escrow" -p bet.bos
```

-- Approve MSIG (for BPs)
```
bosc tx create escrow.bos approve '{"escrow_name": "<NAME>", "approver":"eosio"}' -p eosio --skip-sign --expiration 36000 --write-transaction approve.json
```

-- Propose MSIG (for BPs)
```
bosc multisig propose <PROPOSER> <PROPOSAL NAME> approve.json --request-producers
```

-- BP approve (for BPs)
```
bosc tx create escrow.bos approve '{"escrow_name":"<NAME>","approver":"eosio"}' -p <BP ACCOUNT>
```

-- Exec MSIG (for BPs)
```
bosc multisig exec <PROPOSER> <PROSOAL NAME> <EXECUTER>
```

--90% Claim Escrow  (for proposer)
```
bosc tx create escrow.bos claim '{"escrow_name":"<NAME>"}' -p <ACCOUNT>
```

--10% Transfer to each BPs and auditors manually  (for BET)

```
bosc tx create eosio.token transfer '{"from": "escrow.bos", "to":<ACCOUNT>,"quantity":"0.9000 BOS","memo":"Thanks for your opinions"}' -p bet.bos --skip-sign --expiration 36000 --write-transaction approve.json
```

-- Propose MSIG (for BET)
```
bosc multisig propose <PROPOSER> <PROPOSAL NAME> approve.json --request <BET ACCOUNT>CC,<BET ACCOUNT>...(7 accounts)
```

-- MSIG approve (for BET)
```
bosc multisig approve <PROPOSER> <PROPOSAL NAME> <APPROVER> 
```

-- Exec MSIG (for BET)
```
bosc multisig exec <PROPOSER> <PROSOAL NAME> 
```
