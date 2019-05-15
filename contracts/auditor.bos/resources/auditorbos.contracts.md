<h1 class="contract">updatebio</h1>

## Description

To allow a candidate update their bio information after they have nominated. The action ensures the user has agreed to the latest terms and conditions, has the correct authorization of the {{ cand }} to perform the action and is already nominated as a candidate. Then the bio information for the candidate will be updated leaving all other data of the candidate unchanged.

<h1 class="contract">fireauditor</h1>

## Description

To allow elected auditors to (where quorum and configured majorities are met) to remove a fellow auditor and lock up their tokens until the configured delay period has passed.

<h1 class="contract">resign</h1>

## Description

To remove an elected auditor. This action must be run by the resigning auditor and the outcome should remove the elected auditor and lock up their tokens until the delay period has passed so the tokens can be claimed with the unstake action.

<h1 class="contract">firecand</h1>

## Description

The intent of forehand is to set a candidate to a state of inactive so they will be excluded from the next election round. This action may only be run by the by elected auditors (where quorum and configured majorities are met). There is an option to lock up the candidate's tokens until a delay period has passed based on the delay set in the config after which the tokens can be claimed with the unstake action. If the option passed is false and there is an existing lockup delay on the tokens then this lockup will continue to be active until the lock up time has passed.

<h1 class="contract">unstake</h1>

## Description

To return staked tokens back to the candidate if the user is no longer an active candidate and there is no delay set on the candidate the staked tokens will be returned to the candidate.
**TERM:** This action lasts for the duration of the time taken to process the transaction.

<h1 class="contract">updateconfig</h1>

Update the configuration for the running contract of selected parameters without needing change the source code. This requires a privileged account.

<h1 class="contract">nominatecand</h1>

### V1.0 Auditor’s Declaration of Independence and Impartiality

I, {{ nominatecand }}, accept to serve as Auditor, in accordance with the BOS Rules.

I
**DECLARE** to be and to intend to remain independent and impartial during the auditing procedure.

**DECLARE** that, to my knowledge, there are no facts, circumstances or relationships which may affect my independence and impartiality.

**DECLARE** to treat all BOS members fairly, reward contributions appropriately and not seek unmerited profits. No member should have less or more information about an auditing decision than others.

**DECLARE** not to seek any stake in, or exert undue influence over, other block producers and shall take appropriate measures to protect my own independence and impartiality.

## Description

The intent of {{ nominatecand }} is to nominates a candidate to auditor election, Accounts must nominate as a candidate before they can be voted for. The candidate must lock a configurable number of tokens before trying to nominate (configurable via {{ updateconfig }} in the parameter lockupasset which will be sent from the token contract as defined and set in the code of the contract. If a user previously been a candidate they may have enough staked tokens to not require further staking but will otherwise need to transfer the difference to meet the required stake.

<h1 class="contract">withdrawcand</h1>

## Description

To withdraw a candidate for becoming an elected auditor. The action ensures the {{ cand }} account is currently nominated. On success the amount of tokens that was locked up via the {{ nominatecand }} action will be added to a list of pending transactions to transfer back to the {{ cand }} account. The actual transfer would be performed by a separate action due to the auth requirement for sending funds from the contract's account.

<h1 class="contract">voteauditor</h1>

## Description

To allow a member of BOS to vote for candidates that are eligible become auditors after the next call to {{ newtenure }}. The action ensures the user has agreed to the latest terms and conditions and has the correct authorization of the account: {{ voter }} to place or change an active vote. Upon success this action will either update an existing vote with a new set of candidates or create a new active vote for the {{ voter }} for candidates eligible for election.

<h1 class="contract">newtenure</h1>

## Description

To signal the end of one election period and commence the next. It performs several actions after the following conditions are met:

- The action is not called before the period should have ended
- Enough voter value has participated to trigger the initial running of the BOS
- After BOS auditors has started enough voter value has continued engagement with the BOS auditor voting process.

1. Set the permissions for the elected auditors so they have sufficient permission to run the BOS auditor permission according to the constitution and technical permissions design.
2. Set the time for the beginning of the next period to mark the reset anniversary for the BOS auditor elections.

<h1 class="contract">claimpay</h1>

## Description

The intent of {{ claimpay }} is to allow an account to claim pending payment amounts due to the account. The pay claim they are claiming needs to be visible in the `pendingpay` table. Transfers to the claimer via an inline transfer on the `eosio.token` contract and then removes the pending payment record from the `pending_pay` table. The active auth of this claimer is required to complete this action.

<h1 class="contract">refreshvote</h1>

## Description

To update the auditor's vote weight.
