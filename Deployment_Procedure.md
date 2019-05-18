## 1. Account Creation and Authority

### eosio.forum

```
owner: eosio
active: eosio
```

### bet.bos

```
owner:  eosio@active
active: (7/25) bet
```

### escrow.bos

```
owner: eosio@active
active:
        escrew.bos@eosio.code
        bet.bos@active
        eosio@active
```

### auditor.bos

```
owner: eosio@active
active:
        auditor.bos@eosio.code
        bet.bos@active
        eosio@active
auditors        
```





## 2. Set Contract and Set Permission

Create `escrow.bos`, set contract, and set permission

Create `auditor.bos`, set contract, and set permission

Create `eosio.forum`, set contract, and set permission



### 2.1 Create 4 Accounts

```
cleos -u https://bos.eoshenzhen.io:9443 system newaccount eosio bet.bos EOS7QA2LEZFjTEs43snHMmtVabCvtUDzLChbFdQ9TP717LwDyQJbF EOS5oNwXoHRXLCrzYLqkRK7wx1UvwZZnGJGcQz6bXPEYv1mRhBbMf --stake-cpu "10.0000 BOS" --stake-net "10.0000 BOS" --buy-ram "20.0000 BOS" -p eosio -s -j -d > newacct_betbos.json


cleos -u https://bos.eoshenzhen.io:9443 system newaccount  eosio  escrow.bos EOS7oPEjU1nGKNoN7iCGmuj67P5Wd5vFCQicvpHNjE3Jhfr6Hz4hk  EOS7h6ASi8oDqtgNa8wbgETMVvn4EkqA9bopLcjoDdVuoee4CKgRT   --stake-cpu "10.0000 BOS" --stake-net "10.0000 BOS" --buy-ram "20.0000 BOS" -p eosio  -s -j -d > newacct_escrowbos.json


cleos -u https://bos.eoshenzhen.io:9443 system newaccount  eosio  auditor.bos EOS5UEAtJAwTqX3puiitoTPEbRiXxkMnG3xBJTK5x7i4ND62pRzGG  EOS54wzoGXqcgUDsZxDvW4q39UBL4VNC37dD7NjE392rxaht9eBTB  --stake-cpu "10.0000 BOS" --stake-net "10.0000 BOS" --buy-ram "20.0000 BOS" -p eosio  -s -j -d > newacct_auditorbfos.json


cleos -u https://bos.eoshenzhen.io:9443 system newaccount eosio eosio.forum EOS5Qqi6f4atV3sXWBxK6MsK53ELJ2cnbo3KZRnu54X3kgdJgR6u2 --stake-cpu "10.0000 BOS" --stake-net "10.0000 BOS" --buy-ram "20.0000 BOS" -p eosio -s -j -d > newacct_eosioforum.js
```



### 2.2 multisig propose_trx

```
cleos -u https://bos.eoshenzhen.io:9443 multisig propose_trx createbetbos bppermission.json newacct_betbos.json pursonchen22 


cleos -u https://bos.eoshenzhen.io:9443 multisig propose_trx createescrow bppermission.json  newacct_escrowbos.json pursonchen22  


cleos -u https://bos.eoshenzhen.io:9443 multisig propose_trx createaudito  bppermission.json  newacct_auditorbfos.json pursonchen22  


cleos -u https://bos.eoshenzhen.io:9443 multisig propose_trx 
createforum bppermission.json  newacct_eosioforum.json pursonchen
```





### 2.3 BP APPROVE

```
cleos -u https://bos.eoshenzhen.io:9443 multisig approve createbetbos '{"actor":"BP_name","permission":"active"}' -p BP_name


cleos -u https://bos.eoshenzhen.io:9443 multisig approve createaudito '{"actor":"BP_name","permission":"active"}' -p BP_name


cleos -u https://bos.eoshenzhen.io:9443 multisig approve createforum '{"actor":"BP_name","permission":"active"}' -p BP_name


cleos -u https://bos.eoshenzhen.io:9443 multisig approve createescrow '{"actor":"BP_name","permission":"active"}' -p BP_na
```



### 2.4  multisig exec proposal

```
cleos -u https://bos.eoshenzhen.io:9443 multisig exec pursonpurson createbetbos -p pursonpurson@active


cleos -u https://bos.eoshenzhen.io:9443 multisig exec pursonpurson createaudito -p pursonpurson@active


cleos -u https://bos.eoshenzhen.io:9443 multisig exec pursonpurson createescrow -p pursonpurson@active


cleos -u https://bos.eoshenzhen.io:9443 multisig exec pursonpurson createforum -p pursonpurson@acti
```



## 3. Deploy 3 Contracts

### `eosio.forum`, `auditor.bos`, `escrow.bos` 

```
cleos -u https://bos.eoshenzhen.io:9443 set contract eosio.forum eosio.forum -p eosio.forum@active


cleos -u https://bos.eoshenzhen.io:9443 set contract auditor.bos auditor.bos  -p  auditor.bos@active


cleos -u https://bos.eoshenzhen.io:9443 set contract escrow.bos escrow.bos  -p  escrow.bos@activ
```

## 4. Create Account `bet.bos`

### 4.1 active  7/25

```
cleos -u https://bos.eoshenzhen.io:9443 set account permission bet.bos active '{"threshold":7,"keys":[],"waits":[],"accounts":[{"permission": {"actor":"kesaritooooo","permission":"active"},"weight": 1}, {"permission": {"actor":"chengsong111","permission":"active"},"weight": 1}, {"permission": {"actor":"tyeeeeeeeeee","permission":"active"},"weight": 1}, {"permission": {"actor":"iwangyunpeng","permission":"active"},"weight": 1}, {"permission": {"actor":"igor","permission":"active"},"weight": 1}, {"permission": {"actor":"mybabylilian","permission":"active"},"weight": 1}, {"permission": {"actor":"bosmasterdin","permission":"active"},"weight": 1}, {"permission": {"actor":"vitojingchen","permission":"active"},"weight": 1}, {"permission": {"actor":"sheldonhuang","permission":"active"},"weight": 1}, {"permission": {"actor":"catherine521","permission":"active"},"weight": 1}, {"permission": {"actor":"larosenonaka","permission":"active"},"weight": 1}, {"permission": {"actor":"winlinwinlin","permission":"active"},"weight": 1}, {"permission": {"actor":"mike","permission":"active"},"weight": 1}, {"permission": {"actor":"rohananswers","permission":"active"},"weight": 1}, {"permission": {"actor":"breaktherule","permission":"active"},"weight": 1}, {"permission": {"actor":"angelcats123","permission":"active"},"weight": 1}, {"permission": {"actor":"leonsunlucky","permission":"active"},"weight": 1}, {"permission": {"actor":"angeliazhang","permission":"active"},"weight": 1}, {"permission": {"actor":"ckqdbwzpmtqz","permission":"active"},"weight": 1}, {"permission": {"actor":"blokspartner","permission":"active"},"weight": 1}, {"permission": {"actor":"eric","permission":"active"},"weight": 1}, {"permission": {"actor":"pursonpurson","permission":"active"},"weight": 1}, {"permission": {"actor":"victorleeosx","permission":"active"},"weight": 1}, {"permission": {"actor":"deadlock2bos","permission":"active"},"weight": 1}, {"permission": {"actor":"deniscarrier","permission":"active"},"weight": 1]}' -p bet.bos@owner
```

### 4.2 分配owner权限 3/7

```
cleos -u https://bos.eoshenzhen.io:9443 set account permission bet.bos owner '{"threshold":1,"keys":"","accounts":[{"permission": {"actor":"eosio","permission":"active"},"weight":1}]}' -p bet.bos@owner
```



## 5. Create Account `eosio.forum`  

### 5.1 set active and owner key to `eosio`

```
cleos -u  https://bos.eoshenzhen.io:9443 set account permission eosio.forum active '{"threshold":1 ,"keys":[],"waits":[],"accounts":[{"weight": 1, "permission": {"actor": "eosio", "permission": active}}]}'  -p eosio.forum@owner


cleos -u  https://bos.eoshenzhen.io:9443 set account permission eosio.forum owner '{"threshold":1 ,"keys":[],"waits":[],"accounts":[{"weight": 1, "permission": {"actor": "eosio", "permission": active}}]}'  -p eosio.forum@owner
```





## 6. auditor.bos

### 6.1 set active key to `eosio`, `bet.bos` and `auditors`

```
cleos -u  https://bos.eoshenzhen.io:9443 set account permission auditor.bos active '{"threshold":1 ,"keys":[],"waits":[],"accounts":[{"weight": 1, "permission": {"actor": "eosio", "permission": active},
{"permission": {"actor": "bet.bos", "permission": "active"}, "weight": 1},
{"permission": {"actor": "auditor.bos", "permission": "active"}, "weight": 1}
]}'  -p auditor.bos@active
```



### 6.2 set owner key to `eosio`

```
cleos -u  https://bos.eoshenzhen.io:9443 set account permission auditor.bos owner '{"threshold":1 ,"keys":[],"waits":[],"accounts":{"weight": 1, "permission": {"actor": "eosio", "permission": active}}}'  -p auditor.bos@owner
```



## 7. escrow.bos

### 7.1 set active key to `eosio`, `bet.bos` and `auditors`

```
cleos -u  https://bos.eoshenzhen.io:9443 set account permission auditor.bos active '{"threshold":1 ,"keys":[],"waits":[],"accounts":{"weight": 1, "permission": {"actor": "eosio", "permission": active}},
{"permission": {"actor": "bet.bos", "permission": "active"}, "weight": 1},
{"permission": {"actor": "escrow.bos", "permission": "active"}, "weight": 1},
}'  -p auditor.bos@active
```



### 7.2 set owner key to `eosio`

```
cleos -u  https://bos.eoshenzhen.io:9443 set account permission auditor.bos owner '{"threshold":1 ,"keys":[],"waits":[],"accounts":{"weight": 1, "permission": {"actor": "eosio", "permission": active}}}'  -p auditor.bos@owner
```
