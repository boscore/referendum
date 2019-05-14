# `auditor.bos` - BOS Auditor Elections Contract

This contract will be in charge of auditor registration and voting for BOS auditors.  It will also contain a function which could be called periodically to update the auditor set, and allocate payments.

When an auditor registers, they need to provide their bio by calling the `updatebio` action.

If an elected auditor resigns via the `withdrawcand` during a period a new candidate will be chosen to fill the gap on the auditor board from the votes ranking in the candidates at that moment.

## Quickstart

### Nominate Candidante

> Must `eosio::transfer` BOS tokens at a minimum of `lockupasset` before submitting `nominatecand` action

> Once candidate is nominated, BOS users can now vote for that candidate to become a BOS auditor.

```
$ eosc transfer <CANDIDATE> auditor.bos "100.0000 BOS" -m "stake for auditor.bos"
$ eosc tx create auditor.bos nominatecand '{"cand": "<CANDIDATE>"}' -p <CANDIDATE>@active
```

### Vote for Auditor Candidate

```
$ eosc tx create auditor.bos voteauditor '{"voter":"<VOTER>","newvotes":["<CANDIDATE_1>", "<CANDIDATE_2>","<CANDIDATE_3>"]}' -p deniscarrier
```

### Withdraw Candidate & Unstake

> A Candidate must `withdrawcand` to be allowed to `unstake` any BOS assets that was sent to `auditor.bos`.

```
$ eosc tx create auditor.bos withdrawcand '{"cand":"<CANDIDATE>"}' -p <CANDIDATE>
$ eosc tx create auditor.bos unstake '{"cand":"<CANDIDATE>"}' -p <CANDIDATE>
```

### Resign as Auditor

> Removes elected auditor from `auditor.bos@auditors`

```
$ eosc tx create auditor.bos resign '{"auditor":"<AUDITOR>"}' -p <AUDITOR>
```

### Start Auditor Tenure (Election)

> `auditor.bos@auditors` permission will `updateauth` with candidates with the highest votes

```bash
$ eosc tx create auditor.bos newtenure '{"message":"newtenure for auditor.bos"}' -p auditor.bos@active
```

### Fire Auditor

> Removes Auditor from `auditor.bos@auditors` authority

```bash
$ eosc tx create auditor.bos fireauditor '{"auditor": "<AUDITOR NAME>"}' -p auditor.bos@active
```

### Resign as Auditor

> Removes Auditor from `auditor.bos@auditors` authority

```bash
$ eosc tx create auditor.bos resign '{"auditor": "<AUDITOR NAME>"}' -p <AUDITOR NAME>@active
```

## Tables

### candidates

- candidate_name (name)   - Account name of the candidate (INDEX)
- is_active (int8) - Boolean indicating if the candidate is currently available for election. (INDEX)
- locked_tokens (asset) - An asset object representing the number of tokens locked when registering
- total_votes (uint64) - Updated tally of the number of votes cast to a candidate. This is updated and used as part of the `newtenure` calculations. It is updated every time there is a vote change or a change of token balance for a voter for this candidate to facilitate live voting stats.

### auditors

- auditor_name (name) - Account name of the auditor (INDEX)
- total_votes - Tally of the number of votes cast to a auditor when they were elected in. This is updated as part of the `newtenure` action.

### votes

- voter (account_name) - The account name of the voter (INDEX)
- candidates (account_name[]) - The candidates voted for, can supply up to the maximum number of votes (currently 5) - Can be configured via `updateconfig`

### pendingpay

- key (uint64) 				-  auto incrementing id to identify a payment due to an auditor
- receiver (account_name) 	- The account name of the intended receiver.
- quantity (asset)         - The amount for the payment.
- memo (string)            - A string used in the memo to help the receiver identify it in logs.

### config

- lockupasset (asset) -  The amount of assets that are locked up by each candidate applying for election.
- maxvotes (int default=3) - The maximum number of votes that each member can make for a candidate.
- numelected (int default=5) -  Number of auditors to be elected for each election count.
- auditor_tenure (uint32 =  90 * 24 * 60 * 60) - Length of a period in seconds. Used for pay calculations if an early election is called and to trigger deferred `newtenure` calls.
- authaccount ( account= "auditor.bos") - account to have active auth set with all auditors on the newtenure.
- initial_vote_quorum_percent (uint32) - Amount of token value in votes required to trigger the initial set of auditors
- vote_quorum_percent (uint32) - Amount of token value in votes required to trigger the allow a new set of auditors to be set after the initial threshold has been achieved.
- auth_threshold_auditors (uint8) - Number of auditors required to approve the lowest level actions.
- lockup_release_time_delay (date) - The time before locked up stake can be released back to the candidate using the unstake action

## Actions

---
### nominatecand

This action is used to nominate a candidate for auditor elections. It must be authorised by the candidate and the candidate must be an active member of BOS, having agreed to the latest constitution. The candidate must have transferred a number of tokens (determined by a config setting - `lockupasset`) to the contract for staking before this action is executed. This could have been from a recent transfer with the contract name in the memo or from a previous time when this account had nominated, as long as the candidate had never `unstake`d those tokens.

##### Assertions:

-   The account performing the action is authorised.
-   The candidate is not already a nominated candidate.
-   The requested pay amount is not more than the config max amount.
-   The requested pay symbol type is the same from config max amount ( The contract supports only one token symbol for payment).
-   The candidate is currently a member or has agreed to the latest constitution.
-   The candidate has transferred sufficient funds for staking if they are a new candidate.
-   The candidate has enough staked if they are re-nominating as a candidate and the required stake has changed since they last nominated.

##### Parameters:

    cand  			- The account id for the candidate nominating.

##### Post Condition:

The candidate should be present in the candidates table and be set to active. If they are a returning candidate they should be set to active again. The `locked_tokens` value should reflect the total of the tokens they have transferred to the contract for staking. The number of active candidates in the contract will be incremented.

---
### withdrawcand

This action is used to withdraw a candidate from being active for auditor elections.

#### Assertions:

-   The account performing the action is authorised.
-   The candidate is already a nominated candidate.

##### Parameters:

    cand  - The account id for the candidate nominating.

##### Post Condition:

The candidate should still be present in the candidates table and be set to inactive. If the were recently an elected auditor there may be a time delay on when they can unstake their tokens from the contract. If not they will be able to unstake their tokens immediately using the unstake action.

---
### resign

This action is used to resign as an auditor.

##### Assertions:

-   The `auditor` account performing the action is authorised to do so.
-   The `auditor` account is currently an elected auditor.

##### Parameters:

    auditor  - The account id for the candidate nominating.

##### Post Condition:

The auditor will be removed from the active auditors and should still be present in the candidates table but will be set to inactive. Their staked tokens will be locked up for the time delay added from the moment this action was called so they will not able to unstake until that time has passed. A replacement auditor will be selected from the candidates to fill the missing place (based on vote ranking) then the auths for the controlling BOS auditor auth account will be set for the auditor board.

---

### updatebio

Update the bio for this candidate / auditor. This will be available on the account immediately in preparation for the next election cycle.

##### Assertions:

 - the message has the permission of the account registering.
 - the account has agreed to the current terms.
 - the `cand` account is currently registered.
 - the length of the bio must be less than 256 characters.

 ##### Parameters

    cand - The account id for updating profile
    bio - Bio content

---

### voteauditor

This action is to facilitate voting for candidates to become auditors of BOS. Each member will be able to vote a configurable number of auditors set by the contract configuration. When a voter calls this action either a new vote will be recorded or the existing vote for that voter will be modified. If an empty array of candidates is passed to the action an existing vote for that voter will be removed.

##### Assertions:

-   The voter account performing the action is authorised to do so.
-   The voter account performing has agreed to the latest member terms for BOS.
-   The number of candidates in the newvotes vector is not greater than the number of allowed votes per voter as set by the contract config.
-   Ensure there are no duplicate candidates in the voting vector.
-   Ensure all the candidates in the vector are registered and active candidates.

#### Parameters:

    voter     - The account id for the voter account.
    newvotes  - A vector of account ids for the candidate that the voter is voting for.

##### Post Condition:

An active vote record for the voter will have been created or modified to reflect the newvotes. Each of the candidates will have their total_votes amount updated to reflect the delta in voter's token balance. Eg. If a voter has 1000 tokens and votes for 5 candidates, each of those candidates will have their total_votes value increased by 1000. Then if they change their votes to now vote 2 different candidates while keeping the other 3 the same there would be a change of -1000 for 2 old candidates +1000 for 2 new candidates and the other 3 will remain unchanged.

---

### refreshvote

Refresh vote since `eosio` does not notify this contract

#### Parameters:

    voter - The account id for the voter account.

---
### updateconfig

Updates the contract configuration parameters to allow changes without needing to redeploy the source code.

#### Message

updateconfig(<params>)

This action asserts:

 - the message has the permission of the contract account.
 - the supplied asset symbol matches the current lockup symbol if it has been previously set or that there have been no 	.

The parameters are:

- lockupasset(uint8_t) : defines the asset and amount required for a user to register as a candidate. This is the amount that will be locked up until the user calls `withdrawcand` in order to get the asset returned to them. If there are currently already registered candidates in the contract this cannot be changed to a different asset type because of introduced complexity of handling the staked amounts.
- maxvotes(asset) : Defines the maximum number of candidates a user can vote for at any given time.
- numelected(uint16_t) : The number of candidates to elect for auditors. This is used for the payment amount to auditors for median amount.
- auditor_tenure(uint32_t) : The length of a period in seconds. This is used for the scheduling of the deferred `newtenure` actions at the end of processing the current one. Also is used as part of the partial payment to auditors in the case of an elected auditor resigning which would also trigger a `newtenure` action.
- authaccount(name) : The managing account that controls the BOS auditor permission.
- initial_vote_quorum_percent (uint32) : The percent of voters required to activate BOS for the first election period.
- vote_quorum_percent (uint32) : The percent of voters required to continue BOS for the following election periods after the first one has activated BOS.
- auth_threshold_auditors (uint8) : The number of auditors required to approve an action in the low permission category ( ordinary action such as a worker proposal).

---

### newtenure

This action is to be run to end and begin each period in BOS life cycle. It performs multiple tasks for BOS including:

-   Allocate auditors from the candidates tables based on those with most votes at the moment this action is run. -- This action removes and selects a full set of auditors each time it is successfully run selected from the candidates with the most votes weight. If there are not enough eligible candidates to satisfy BOS config numbers the action adds the highest voted candidates as auditors as long their votes weight is greater than 0. At this time the held stake for the departing auditors is set to have a time delayed lockup to prevent the funds from releasing too soon after each auditor has been in office.
-   Distribute pay for the existing auditors based on the configs into the pending pay table so it can be claimed by individual candidates. -- The pay is distributed as determined by the median pay of the currently elected auditors. Therefore all elected auditors receive the same pay amount.
-   Set BOS auths for the intended controlling accounts based on the configs thresholds with the newly elected auditors. This action asserts unless the following conditions have been met:
-   The action cannot be called multiple times within the period since the last time it was previously run successfully. This minimum time between allowed calls is configured by the period length parameter in contract configs.
-   To run for the first time a minimum threshold of voter engagement must be satisfied. This is configured by the `initial_vote_quorum_percent` field in the contract config with the percentage calculated from the amount of registered votes cast by voters against the max supply of tokens for BOS's primary currency.
-   After the initial vote quorum percent has been reached subsequent calls to this action will require a minimum of `vote_quorum_percent` to vote for the votes to be considered sufficient to trigger a new period with new auditors.

 ##### Parameters:

     message - a string that is used to log a message in the chain history logs. It serves no function in the contract logic.

---

### claimpay

This action is to claim pay as a auditor.

##### Assertions:

-   The caller to the action account performing the action is authorised to do so.
-   The payid is for a valid pay record in the pending pay table.
-   The caller account is the same as the intended destination account for the pay record.

##### Parameters:

     payid - The id for the pay record to claim from the pending pay table.

 Post Condition:

The quantity owed to the auditor as referred to by the pay record is transferred to the claimer and then the pay record is removed from the pending pay table.

---
### unstake

This action is used to unstake a candidates tokens and have them transferred to their account.

##### Assertions:

-   The candidate was a nominated candidate at some point in the past.
-   The candidate is not already a nominated candidate.
-   The tokens held under candidate's account are not currently locked in a time delay.

##### Parameters:

    cand  - The account id for the candidate nominating.

### Post Condition:

The candidate should still be present in the candidates table and should be still set to inactive. The candidates tokens will be transferred back to their account and their `locked_tokens` value will be reduced to 0.

---
### firecand

This action is used to remove a candidate from being a candidate for auditor elections.

## Assertions:

-   The action is authorised by the mid level permission the auth account for the contract.
-   The candidate is already a nominated candidate.

##### Parameters

     cand        - The account id for the candidate nominating.
     lockupStake - if true the stake will be locked up for a time period as set by the contract config `lockup_release_time_delay`

##### Post Condition:

The candidate should still be present in the candidates table and be set to inactive. If the `lockupstake` parameter is true the stake will be locked until the time delay has passed. If not the candidate will be able to unstake their tokens immediately using the unstake action to have them returned.

---
### fireauditor

This action is used to remove a auditor.

##### Assertions:

-   The action is authorised by the mid level of the auth account (currently elected auditor board).
-   The `auditor` account is currently an elected auditor.

##### Parameters:

     cand - The account id for the candidate nominating.

##### Post Condition:

The auditor will be removed from the active auditors and should still be present in the candidates table but will be set to inactive. Their staked tokens will be locked up for the time delay added from the moment this action was called so they will not able to unstake until that time has passed. A replacement auditor will be selected from the candidates to fill the missing place (based on vote ranking) then the auths for the controlling BOS auditor auth account will be set for the auditor board.

---

# Compile


The contract code has some compile time constants used for configuration. As a compile time constant the code has more flexibility for reuse, and an extra layer of safety over exposing another configuration variable which could be changed after the code has been set and the ability to unit test the code without needing to modify the source just for testing.
The available compile time flags are:

- TOKENCONTRACT (default = "eosio.token") - This is to set the associated token contract to inter-operate with for tracking voting weights, registered members and staking.
- VOTING_DISABLED (default = false) - Setting this flag will disable the ability for anyone to vote for auditors by disabling the vote action.

When put all together a compile command with all the bells and whistles might look like:

```bash
eosio-cpp -DTOKENCONTRACT=eosio.token -DVOTING_DISABLED -o auditor.wasm auditor.cpp
```

> **Note:** Since there are default values for the above flags they do not all need to be included to compile successfully.

---

# Tests

The repo includes automated tests to exercise the main action paths in the contract against a running local node.
The tests are included in the tests folder as `rspec` tests in Ruby.

### Installation

To run the tests you would first need:
- `ruby 2.4.1` or later installed.
- Ruby gems as specified in the `Gemfile`.
    - These can be installed by running `bundle install` from the project root directory.
Eos installed locally that can be launched via  `nodeos`

There is one action that requires `ttab` which is a nodejs module but this can be easily avoided by a small modification to the rspec tests as detailed in the file. The purpose of using ttab is to start a second tab running nodeos to help diagnose bugs during the test run.

### To run the tests:

```bash
run `rspec tests/contract_spec.rb` from the project root.
```
