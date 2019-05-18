## 1. Account Creation and Set Authority

### `eosio.forum`

```
owner: eosio
active: eosio
```

### `bet.bos`

```
owner:  eosio@active
active: (7/25) bet
```

### `escrow.bos`

```
owner: eosio@active
active:
        escrew.bos@eosio.code
        bet.bos@active
        eosio@active
```

### `auditor.bos`

```
owner: eosio@active
active:
        auditor.bos@eosio.code
        bet.bos@active
        eosio@active
auditors        
```





## 2. Set Contract and Set Permission

Create `bet.bos`, and set permission

Create `escrow.bos`, set contract, and set permission

Create `auditor.bos`, set contract, and set permission

Create `eosio.forum`, set contract, and set permission



### 2.0 Create the permission struct yaml

#### 2.0.1 bet.bos.yaml

```yaml
owner:
  threshold: 1
  accounts:
  - permission:
      actor: eosio
      permission: active
    weight: 1
active:
  threshold: 7
  accounts:
  - permission:
      actor: angelcats123
      permission: active
    weight: 1
  - permission:
      actor: angeliazhang
      permission: active
    weight: 1
  - permission:
      actor: blokspartner
      permission: active
    weight: 1
  - permission:
      actor: bosmasterdin
      permission: active
    weight: 1
  - permission:
      actor: breaktherule
      permission: active
    weight: 1
  - permission:
      actor: catherine521
      permission: active
    weight: 1
  - permission:
      actor: chengsong111
      permission: active
    weight: 1
  - permission:
      actor: ckqdbwzpmtqz
      permission: active
    weight: 1
  - permission:
      actor: deadlock2bos
      permission: active
    weight: 1
  - permission:
      actor: deniscarrier
      permission: active
    weight: 1
  - permission:
      actor: eric
      permission: active
    weight: 1
  - permission:
      actor: igor
      permission: active
    weight: 1
  - permission:
      actor: iwangyunpeng
      permission: active
    weight: 1
  - permission:
      actor: kesaritooooo
      permission: active
    weight: 1
  - permission:
      actor: larosenonaka
      permission: active
    weight: 1
  - permission:
      actor: leonsunlucky
      permission: active
    weight: 1
  - permission:
      actor: mike
      permission: active
    weight: 1
  - permission:
      actor: mybabylilian
      permission: active
    weight: 1
  - permission:
      actor: pursonchen22
      permission: active
    weight: 1
  - permission:
      actor: rohananswers
      permission: active
    weight: 1
  - permission:
      actor: sheldonhuang
      permission: active
    weight: 1
  - permission:
      actor: tyeeeeeeeeee
      permission: active
    weight: 1
  - permission:
      actor: victorleeosx
      permission: active
    weight: 1
  - permission:
      actor: vitojingchen
      permission: active
    weight: 1
  - permission:
      actor: winlinwinlin
      permission: active
    weight: 1
```

#### 2.0.2 eosio.forum.yaml

```YAMl
owner:
  threshold: 1
  accounts:
  - permission:
      actor: eosio
      permission: active
    weight: 1
active:
  threshold: 1
  accounts:
  - permission:
      actor: eosio
      permission: active
    weight: 1
```

#### 2.0.3 escrow.bos.yaml

```shell
owner:
  threshold: 1
  accounts:
  - permission:
      actor: eosio
      permission: active
    weight: 1
active:
  threshold: 1
  accounts:
  - permission:
      actor: eosio
      permission: active
    weight: 1
  - permission:
      actor: bet.bos
      permission: active
    weight: 1
  - permission:
      actor: escrow.bos
      permission: active
    weight: 1
```

#### 2.0.4 auditor.bos.yaml

```shell
owner:
  threshold: 1
  accounts:
  - permission:
      actor: eosio
      permission: active
    weight: 1
active:
  threshold: 1
  accounts:
  - permission:
      actor: eosio
      permission: active
    weight: 1
  - permission:
      actor: bet.bos
      permission: active
    weight: 1
  - permission:
      actor: auditor.bos
      permission: active
    weight: 1
```

​		

### 2.1 Create 4 Accounts with eosc

```shell
eosc -u https://bos.eoshenzhen.io:9443 system newaccount eosio bet.bos --stake-net "10.0000 BOS" --stake-cpu "10.0000 BOS" --transfer --buy-ram-kbytes 100 --skip-sign --expiration 604800 --write-transaction create_betbos.json --auth-file bet.bos.yml

eosc -u https://bos.eoshenzhen.io:9443 system newaccount eosio escrow.bos --stake-net "10.0000 BOS" --stake-cpu "10.0000 BOS" --transfer --buy-ram-kbytes 100 --skip-sign --expiration 604800 --write-transaction create_escrowbos.json --auth-filer escrow.bos.yml

eosc -u https://bos.eoshenzhen.io:9443 system newaccount eosio eosio.forum --stake-net "10.0000 BOS" --stake-cpu "10.0000 BOS" --transfer --buy-ram-kbytes 100 --skip-sign --expiration 604800 --write-transaction create_eosioforum.json --auth-file eosio.forum.yml

eosc -u https://bos.eoshenzhen.io:9443 system newaccount eosio bet.bos --stake-net "10.0000 BOS" --stake-cpu "10.0000 BOS" --transfer --buy-ram-kbytes 100 --skip-sign --expiration 604800 --write-transaction create_auditorbos.json --auth-file auditor.bos.yml
```



### 2.2 multisig propose_trx

```shell
eosc -u https://bos.eoshenzhen.io:9443 multisig propose pursonchen22 createbetbos create_betbos.json --request-producers

eosc -u https://bos.eoshenzhen.io:9443 multisig propose pursonchen22 createescrow create_escrowbos.json --request-producers

eosc -u https://bos.eoshenzhen.io:9443 multisig propose pursonchen22 createforum create_eosioforum.json --request-producers

eosc -u https://bos.eoshenzhen.io:9443 multisig propose pursonchen22 createaudito create_auditorbos.json --request-producers
```



### 2.3 BP APPROVE

```shell
eosc multisig approve pursonchen22 createbetbos <BP ACCOUNT>
eosc multisig approve pursonchen22 createescrow <BP ACCOUNT>
eosc multisig approve pursonchen22 createforum <BP ACCOUNT>
eosc multisig approve pursonchen22 createaudito <BP ACCOUNT>
```



### 2.4  multisig exec proposal

```shell
eosc multisig exec pursonchen22 createbetbos pursonchen22
eosc multisig exec pursonchen22 createescrow pursonchen22
eosc multisig exec pursonchen22 createforum pursonchen22
eosc multisig exec pursonchen22 createaudito pursonchen22
```



## 3. Deploy 3 Contracts

### `eosio.forum`, `auditor.bos`, `escrow.bos` 

```shell
eosc system setcontract eosio.forum eosio.forum eosio.forum  

eosc system setcontract auditor.bos auditor.bos auditor.bos

eosc system setcontract escrow.bos escrow.bos escrow.bos
```



