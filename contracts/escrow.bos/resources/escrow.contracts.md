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

<h1 class="contract">clean</h1>

## Description

To remove all existing escrow agreements for developer purposes. This can only be run with _self permission of the contract which would be unavailable on the main net once the contract permissions are removed for the contract account.