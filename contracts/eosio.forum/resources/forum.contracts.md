<h1 class="contract">post</h1>

## Description

`post` is used to create a post that can either be a parent or
be a response to parent post. They are threaded together using their
unique identifier {{ post_uuid }}.

<h1 class="contract">propose</h1>

## Description

`propose` creates a message on-chain with the intention of receiving
votes from any community members who wish to cast a `vote`.

Each proposal shall be identified with a unique `proposal_name`.

An expiry will be defined in `expires_at`, with {{ expires_at }}
being no later than 6 months in the future.

{{ proposer }} must pay for the RAM to store {{ proposal_name }}, which
will be returned to them once `clnproposal` has been called.

<h1 class="contract">status</h1>

## Description

`status` is used to record a status for the associated {{ account }}.
If the {{ content }} is empty, the action will remove a previous status.
Otherwise, it will add a status entry for the {{ account }} using the
{{ content }} received.

<h1 class="contract">unpost</h1>

## Description

The intent of the `unpost` action is to suggest that a previously posted message (through the `post` action), as referred by {{ post_uuid }}, be removed by the different front-ends reading this contract's transaction flow.

I, {{ poster }} understand that this action will not remove the message from circulation, as it will be imprinted in the blockchain.  I also understand that some front-ends might not remove the message, and even highlight the fact that a message was intended to be removed, potentially attracting attention on an undesired message.

<h1 class="contract">unvote</h1>

## Description

`unvote` allows a user to remove their vote of {{ vote_value }} they have previously
cast on {{ proposal_name }}.

The RAM that was used to store the vote shall be freed-up immediately
after `unvote` has been called by {{ voter }}.

<h1 class="contract">vote</h1>

## Description

I, {{ voter }}, am casting a vote of {{ vote_value }} on {{ proposal_name }}. To change my vote, I may call another `vote` action, with only the most recent `vote` of {{ vote_value }} being the `vote` which I, {{ voter }}, intend to be considered as valid. I acknowledge that using the `unvote` action after placing a `vote` will render my previous `vote` of {{ vote_value }} null and void.

If I, {{ voter }}, have a proxy registered for my on-chain voting, my own `vote` of {{ vote_value }} shall take  precedence over my proxy's `vote`. My stake weight shall be deducted from their voting power and cast as my own.

If I, {{ voter }}, am not the beneficial owner of these tokens, I stipulate I have proof that I’ve been authorized to vote these tokens by their beneficial owner(s).

If I, {{ voter }}, am registered as a proxy and am casting votes on behalf of other users of the blockchain, I acknowledge that I am doing so on their behalf and that they may at any time withdraw their stake weight from my voting power by casting their own `vote` or removing my account as their proxy.

I, {{ voter }}, stipulate I have not and will not accept anything of value in exchange for this `vote`, on penalty of confiscation of these tokens, and other penalties.

<h1 class="contract">cancel</h1>

## Description

`cancel` is used to cancel a {{ proposal_name }} authorized by the {{ proposer }}.
