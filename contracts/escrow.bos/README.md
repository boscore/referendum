# BOS escrow

An escrow contract designed for paying worker proposals.  The intention is that the authority for this account would be nulled.

> Original source code was gratefully authored by [eosDAC](https://github.com/eosdac/dacescrow).

## Quickstart

### Init Escrow

> Only `bet.bos@active` can `init` an escrow.
> This will initialize the escrow between the `sender` & `receiver`.

```bash
$ bosc tx create escrow.bos init '{"sender":"bet.bos","receiver":"<RECEIVER>","approver":"eosio","escrow_name":"<NAME>","expires_at":"2019-09-15T00:00:00","memo":"BOS escrow"}' -p bet.bos
```

### Fund/Initialize Escrow

> BOS funds must be `transfer` into the `escrow.bos` account before starting another escrow

```bash
$ bosc transfer bet.bos escrow.bos "100.0000 BOS" -m "Fund BOS escrow" -p bet.bos
```

### Approve Escrow

> Only `bet.bos@active` or `eosio@active` are allowed to approve.
> if approver is bet.bos, no change, allow proposer to claim 100% of the fund
> if approver is BPs, only keep 90% fund for proposer to claim, and BET.BOS will manually execute transfer ACTION in escrow.bos to send fund to each BPs and each auditors

```bash
$ bosc tx create escrow.bos approve '{"escrow_name":"<NAME>","approver":"eosio"}' -p eosio
```


### Review Escrow

Allows the {{ sender }}, {{ approver }} or {{ receiver }} to perform an on-chain notification which is used to signal a review of the {{ escrow_name }}.

```bash
$ bosc tx create escrow.bos review '{"escrow_name":"<NAME>","user":"<USER>","reviewer":"<REVIEWER>","memo": "review escrow"}' -p <USER>
```

### Claim Escrow

> Executing `claim` will complete the escrow process and transfer the BOS tokens to the receiver.
> Anyone can execute the `claim` action.

```bash
$ bosc tx create escrow.bos claim '{"escrow_name":"<NAME>"}' -p <ACCOUNT>
```

### Proposing `approve` MSIG

1. Create an `approve.json` transaction file which is signed by `eosio@active`

```bash
$ bosc tx create escrow.bos approve '{"escrow_name": "<NAME>", "approver":"eosio"}' -p eosio --skip-sign --expiration 36000 --write-transaction approve.json
```

2. Propose MSIG

```bash
$ bosc multisig propose <PROPOSER> <PROPOSAL NAME> approve.json --request-producers
```

3. Exec MSIG

```bash
$ bosc multisig exec <PROPOSER> <PROSOAL NAME> <EXECUTER>
```

## Caveats

- The sender of an escrow will temporarily be whitelisted to BOS executives. In the future anyone may be a sender
- The sender may only have one unfilled escrow at any given time, however they may have many filled escrows
- To fill an escrow the sender must transfer the `BOS` tokens to this contract. An unfilled escrow will be filled
- The receiver is considered as always approving the escrow. An approval must come from either the sender or the approver
- The sender may only cancel an escrow that has not been filled
- The sender may only refund an escrow that has passed it's expiry
- Unapprove only removes an existing approval, if the action is made before the receiver uses the claim action
- A sender may extend the expiry but not shorten it
- The approver may change extend or shorten the expiry
- The approver may close an escrow. This is essentially the same as refunding it, however without waiting for the expiry to lapse
- The approver may Lock and Unlock an escrow. This prevents ALL actions except unlock and actions made by the approver.

<h1 class="contract">init</h1>

## Description

To create an empty escrow payment agreement for safe and secure funds transfer protecting both {{ sender }} and {{ receiver }} for a determined amount of time.

<h1 class="contract">transfer</h1>

## ACTION: `transfer`

## Description

To listen and react to the eosio.token contract's transfer action and ensure the correct parameters have been included in the transfer action.

<h1 class="contract">approve</h1>

## Description

To approve the release of funds to the intended {{ receiver }}. Each escrow agreement requires at least {{ sender }} or {{ approver }} to grant fund release.

<h1 class="contract">unapprove</h1>

## Description

To unapprove the release of funds to the intended receiver from a previous approved action.

<h1 class="contract">claim</h1>

To claim the escrowed funds for an intended {{ receiver }} after an escrow agreement has met the required approvals.

<h1 class="contract">refund</h1>

To return the escrowed funds back to the original {{ sender }}. This action can only be run after the contract has met the intended expiry time.

<h1 class="contract">cancel</h1>

## Description

To cancel an escrow agreement. This action can only be performed by the {{ sender }} as long as no funds have already been transferred for the escrow agreement. Otherwise they would need to wait for the expiry time and then use the refund action.

<h1 class="contract">extend</h1>

## Description

Allows the sender to extend the expiry

<h1 class="contract">close</h1>

Allows the {{ approver }} to close and refund an unexpired escrow

<h1 class="contract">lock</h1>

## Description

Allows the {{ approver }} to lock an escrow preventing any actions by {{ sender }} or {{ receiver }}.

<h1 class="contract">review</h1>

## Description

Allows the {{ sender }}, {{ approver }} or {{ receiver }} to perform an on-chain notification which is used to signal a review of the {{ escrow_name }}.

<h1 class="contract">clean</h1>

## Description

To remove all existing escrow agreements for developer purposes. This can only be run with _self permission of the contract which would be unavailable on the main net once the contract permissions are removed for the contract account.