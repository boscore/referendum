## A simple forum, messaging and voting system for EOS

[点击查看中文版](./README-cn.md)

The purpose of this contract is to support the EOS Referendum system
by storing proposals and their related votes in-RAM in the blockchain's state.

It's also possible to create related posts and statuses, but they are
not stored in-RAM in the blockchain's state. It allows authenticated messages
to go through, where they are visible in the transaction history of the chain.
Off-chain tools are needed to sort, display, aggregate, and report on the
outputs of the post and status actions.

### Lifecycle

The `propose` action is first called providing the proposer's account, proposal's name (its
id among all other proposals), proposal's title, a JSON string for extra metadata
(specification not defined yet) that can be left empty, and an expiration date
(must be no later than 6 months in the future).

Once the proposal has been created, people can start to vote on it via the `vote` action.
The vote action is called using the voter's account, proposal's name, vote's value (`0` for
negative vote and `1` for a positive vote) and a JSON string for extra metadata
(specification not defined yet), which can be left empty.

A vote overwrites any previous value if present. That means that if you voted initially with a
vote value of `0` (negative vote) and you perform a second `vote` action on the same proposal
this time with a value of `1` (positive vote), your current vote for the proposal is now `1`.

There is no decay once you have voted. Once you vote, it does not change, nor is it removed
until you either call `unvote`.

Once a vote has been cast, a user can remove its vote via the `unvote` action.
The `unvote` action is called using the voter's account and proposal's name. An `unvote`
action completely removes your vote from the proposal and clears the RAM usage
associated to that vote.

### Development

Prerequisites:
- [Docker 17+](https://www.docker.com/get-started) (or [eosio.cdt](https://github.com/EOSIO/eosio.cdt) 1.6+ installed locally)
- [eosc 1.1+](https://github.com/eoscanada/eosc/releases)
- [eos-bios 1.2+](https://github.com/eoscanada/eos-bios/releases)

We assume the `docker` binary is available in your `PATH` environment as well as `eosc` and
`eos-bios`. The `eos-bios` and `eosc` binaries are required to correctly boot the local
development & test node as well as running the automated test suite.

#### Building

Simply call the `build.sh` script which launches a Docker container and
compiles the contract:

```
./build.sh
```

##### Local Toolchain

For compilation purposes, it's possible to use the local [eosio.cdt](https://github.com/EOSIO/eosio.cdt)
instead of pulling a Docker image containing it. Simply call `compile.sh` script
to compile using the local EOSIO.CDT toolchain:

```
./compile.sh
```

#### Running

You can easily start a development node using the `run.sh` script, which uses
[eos-bios](https://github.com/eoscanada/eos-bios) and Docker to launch a
fully configured sandboxed `nodeos` development node:

```
./run.sh
```

This creates the following accounts:
- `eosio.forum`
- `proposer1`
- `proposer2`
- `poster1`
- `poster2`
- `voter1`
- `voter2`
- `zzzzzzzzzzzz`

All accounts created in the development node above use the following public/private
key pair:

- Public: `EOS5MHPYyhjBjnQZejzZHqHewPWhGTfQWSVTWYEhDmJu4SXkzgweP`
- Private: `5JpjqdhVCQTegTjrLtCSXHce7c9M8w7EXYZS7xC13jVFF4Phcrx`

You can pre-fill your environment with some proposals and votes easily by simply
calling the `./tests/data.sh` script.

```
./tests/data.sh
```

Once you are done with the `nodeos` development node, simply call `stop.sh`
to stop the running instance:

```
./stop.sh
```

##### Environment

To easily interact with the development node via `eosc` on your terminal, simply export
the following environment variables:

```
export EOSC_GLOBAL_INSECURE_VAULT_PASSPHRASE="secure"
export EOSC_GLOBAL_API_URL="http://localhost:9898"
export EOSC_GLOBAL_VAULT_FILE="`pwd`/tests/eosc-vault.json"
```

The [direnv](https://direnv.net/) tool can be used to automatically import those variables
when you `cd` in the project's root directory.

#### Tests

Running the full automatic test suite is easy as doing:

```
./tests.sh
```

This launches `nodeos` development node (via `./run.sh`) and then
executes all the integration tests found in `tests` folder (see [all.sh](./tests/all.sh)
for exact files picked up).

To correctly run the tests, you will need to switch the freeze period of
a proposal for 2 seconds (waiting 3 days could be a bit too long!). The `tests.sh`
script takes also care of this changing the freeze period automatically for you
to 2 seconds so it looks like this instead when the tests runs:

```
constexpr static uint32_t FREEZE_PERIOD_IN_SECONDS = 2; // NEVER MERGE LIKE THIS
```

**Important** The `tests.sh` script automatically revert back the changes once the
test script finishes (either in error or successfully). You should check just in case
before sending your changes to be 100% sure that changes were effectively reverted back.
You would not like to push a freeze period of 2 seconds in the repository!

### Deployment

The latest version of this code lives on the `eosio.forum` account on
the EOS Mainnet.

There is also a few accounts that were used for development purposes
as well as for testing updates to the contract. Here the list with some
details about the status:
- `eosforumrcpp` on EOS Mainnet (status: `release candidates`)
- `cancancan345` on Kylin network (status: `unmaintained`)
- `cancancan123` on Kylin network (status: `unmaintained`)
- `eosforumdapp` on EOS Mainnet (status: `unmaintained`)

### Reference

Here is the list of possible actions:

- [propose](#action-propose)
- [cancel](#action-cancel)
- [vote](#action-vote)
- [unvote](#action-unvote)
- [post](#action-post)
- [unpost](#action-unpost)
- [status](#action-status)

#### Action `propose`

Propose a new proposal to the community.

##### Parameters

- `proposer` (type `name`) - The actual proposer's account
- `proposal_name` (type `name`) - The proposal's name, its ID among all proposals
- `title` (type `string`) - The proposal's title (must be less than 1024 characters)
- `proposal_json` (type `string`) - The proposal's JSON metadata, no specification yet, see [Proposal JSON Structure](#proposal-json-structure-guidelines)

##### Rejections

- When missing signature of `proposer`
- When `proposal_name` already exists
- When `title` is longer than 1024 characters
- When `proposal_json` JSON is invalid or too large (must be a JSON object and be less than 32768 characters)

##### Example

```
eosc tx create eosio.forum propose '{"proposer": "proposer1", "proposal_name": "example", "title": "The title, for list views", "proposal_json": ""}' -p proposer1@active
```
OR

```
eosc forum propose proposer1 example "The title, for list views" 2019-01-30T17:03:20 --json "[JSON object]"
```

#### Action `vote`

Vote for a given proposal using your account.

##### Parameters

- `voter` (type `name`) - The actual voter's account
- `proposal_name` (type `name`) - The proposal's name to vote on
- `vote` (type `uint8`) - Your vote on the proposal, `0` means a negative vote, `1` means a positive vote
- `vote_json` (type `string`) - The vote's JSON metadata, no specification yet, see [General JSON Structure Guidelines](#general-json-structure-guidelines)

##### Rejections

- When missing signature of `voter`
- When `proposal_name` does not exist
- When `proposal_name` is already expired
- When the `vote_json` JSON is invalid or too large (must be a JSON object and be less than 8192 characters)

##### Example

```
eosc tx create eosio.forum vote '{"voter": "voter1", "proposal_name": "example", "vote": 0, "vote_json": ""}' -p voter1@active
```
OR
```
eosc forum vote voter1 example 0
```

#### Action `unvote`

Remove your current active vote, effectively reclaiming the stored RAM of the vote. Of course,
your vote will not count anymore (neither positively or negatively) on the current proposal's voting
statistics.

##### Parameters

- `voter` (type `name`) - The actual voter's account
- `proposal_name` (type `name`) - The proposal's name to remove your vote from

##### Rejections

- When missing signature of `voter`
- When `proposal_name` does not exist

##### Example

```
eosc tx create eosio.forum unvote '{"voter": "voter1", "proposal_name": "example"}' -p voter1@active
```
OR
```
eosc forum unvote voter1 example
```

#### Action `cancel`

Is used to cancel a `proposal_name` authorized by the `proposer`.

##### Parameters

- `proposer` (type `name`) - The original proposer
- `proposal_name` (type `name`) - The proposal's name to cancel

##### Rejections

- When missing signatures of proposal's `proposer`
- When `proposal_name` does not exist

##### Example

```
eosc tx create eosio.forum cancel '{"proposer": "proposer1", "proposal_name": "example"}' -p proposer1@active
```

**Note** `proposer1` must be the same as the one that created initially the `example` proposal.

#### Action `post`

##### Parameters

- `poster` (type `name`) - The poster's account
- `post_uuid` (type `string`) - The post `UUID` (for reply purposes)
- `content` (type `string`) - The actual content of the post
- `reply_to_poster` (type `name`) - The initial post's poster your post replies to
- `reply_to_post_uuid` (type `string`) - The initial post's `UUID` your post replies to
- `certify` (type `bool`) - Reserved for future use
- `json_metadata` (type `string`) - The post's JSON metadata, no specification yet, see [General JSON Structure Guidelines](#general-json-structure-guidelines)

##### Rejections

- When missing signature of `poster`
- When `content` is an empty string
- When `content` is bigger than 10240 characters
- When `post_uuid` is an empty string
- When `post_uuid` is bigger than 128 characters
- When `reply_to_poster` is not set but `reply_to_post_uuid` is
- When `reply_to_poster` is not an existing account
- When `reply_to_poster` is set and `reply_to_post_uuid` is an empty string
- When `reply_to_poster` is set and `reply_to_post_uuid` is bigger than 128 characters
- When `json_metadata` JSON is invalid or too large (must be a JSON object and be less than 8192 characters)

##### Example

```
eosc tx create eosio.forum post '{"poster": "poster1", "post_uuid":"examplepost_id", "content": "hello world", "reply_to_poster": "", "reply_to_post_uuid": "", "certify": false, "json_metadata": "{\"type\": \"chat\"}"}' -p poster1@active
```
OR
```
eosc forum post poster1 "hello world"
```

#### Action `unpost`

##### Parameters

- `poster` (type `name`) - Remove a previous post you did
- `post_uuid` (type `string`) - The `UUID` of the post to remove

##### Rejections

- When missing signature of `poster`
- When `post_uuid` is an empty string
- When `post_uuid` is bigger than 128 characters

##### Example

```
eosc tx create eosio.forum unpost '{"poster": "poster1", "post_uuid":"examplepost_id"}' -p poster1@active
```
OR
```
eosc forum unpost poster1 [UUID_of_example]
```

#### Action `status`

Record a status for the associated `account`. If the `content` is empty, the action will remove a
previous status. Otherwise, it will add a status entry for the `account` using the `content` received.

##### Parameters

- `account` (type `name`) - The account to add a status to
- `content` (type `string`) - The content associated to the status

##### Rejections

- When missing signature of `account`
- When `post_uuid` is bigger than 256 characters
- When `content` is the empty string and no previous `status` existed for `account`

Example (add status):

```
eosc tx create eosio.forum status '{"account": "voter2", "content":"status of something"}' -p voter2@active
```

Example (remove previous status):

```
eosc tx create eosio.forum status '{"account": "voter2", "content":""}' -p voter2@active
```
OR
```
eosc forum status voter2 "status of something"
```

Example (remove previous status):

```
eosc forum status voter2 ""
```

#### Table `proposals`

##### Row
- `proposal_name` (type `name`) - The proposal's name, its ID among all proposals
- `proposer` (type `name`) - The actual proposer's account
- `title` (type `string`) - The proposal's title, a brief description of the proposal
- `proposal_json` (type `string`) - The proposal's JSON metadata, no specification yet, see [Proposal JSON Structure Guidelines](#proposal-json-structure-guidelines)
- `created_at` (type `time_point_sec`) - The date at which the proposal's was created, ISO 8601 string format (in UTC) **without** a timezone modifier.

##### Indexes
- First (`1` type `name`) - Index by `proposal_name` field
- Second (`2` type `name`) - Index by `proposer`

##### Example (get all proposals):

```
eosc get table eosio.forum eosio.forum proposal
```
OR
```
eosc forum list
```

##### Example (get all proposals for a given proposer):

**Caveats** Right now, `eosc` does not support searching giving only a direct key. Instead, it really requires
a lower and upper bound. The upper bound being exclusive, to correctly get the upper bound, take the account name
and change the last character to the next one in the EOS name alphabet (order is `a-z1-5.`).

So, looking for all proposals proposed by `testusertest`, the lower bound key would be `testusertest` and
the upper bound key would be `testusertesu` (last character `t` bumped to next one `u`).

```
eosc get table eosio.forum eosio.forum proposal --index 2 --key-type name --lower-bound testusertest --upper-bound testusertesu
```
OR
```
eosc forum list --from-proposer testusertest
```

#### Table `status`

##### Row
- `account` (type `name`) - The status' poster
- `content` (type `string`) - The content of the status
- `updated_at` (type `time_point_sec`) - The date at which the status was last updated, ISO 8601 string format (in UTC) **without** a timezone modifier.

##### Example

```
eosc get table eosio.forum eosio.forum status
```

#### Table `vote`

##### Row
- `id` (type `uint64`) - The unique ID of the `voter`/`proposal_name` pair
- `proposal_name` (type `name`) - The `proposal_name` on which the vote applies
- `voter` (type `name`) - The `voter` that voted
- `vote` (type `uint8`) - The vote value of the `voter` (`0` means negative vote, `1` means a positive vote)
- `vote_json` (type `string`) - The vote's JSON metadata, no specification yet, see [General JSON Structure Guidelines](#general-json-structure-guidelines)
- `updated_at` (type `time_point_sec`) - The date at which the vote was last updated, ISO 8601 string format (in UTC) **without** a timezone modifier.

##### Indexes
- First (`1` type `i64`) - Index by `id` field
- Second (`2` type `i128` input in hexadecimal little-endian format) - Index by proposal name, the key is composed in the high bytes using the `proposal_name` and the low bytes are the `voter`.
- Third (`3` type `i128` input in hexadecimal little-endian format) - Index by voter, the key is composed in the high bytes using the `voter` and the low bytes are the `proposal_name`.

##### Example (get all votes):

```
eosc get table eosio.forum eosio.forum vote
```

##### Example (get all votes for a given proposal):

The idea is to turn the `proposal_name` into an integer, convert it to hexadecimal, and compute the lowest
possible key for `voter` (lower bound) as well as the highest possible key for `voter` (upper bound).

**Note** The hexadecimal values below are all in little-endian format, so high bytes are on the right side
and low bytes on the left side.

Here are the steps to compute the lower/upper bounds for the table query:

 1. Convert `ramusetest` EOS name to hexadecimal using `eosc tools name`.

   ```
   eosc tools names ramusetest

   from \ to  hex               hex_be            name        uint64
   ---------  ---               ------            ----        ------
   name       0040c62a2baca5b9  b9a5ac2b2ac64000  ramusetest  13377287569575133184
   ```

 1. Create the `lower_bound` key by prepending `0000000000000000` to the `hex` value shown
   above: `0x00000000000000000040c62a2baca5b9`

 1. Create the `upper_bound` key by prepending `ffffffffffffffff` to the `hex` value shown
   above: `0xffffffffffffffff0040c62a2baca5b9`.

Now that we have the lower and upper bound keys, simply perform your query:

```
eosc get table eosio.forum eosio.forum vote --index 2 --key-type i128 --lower-bound 0x00000000000000000040c62a2baca5b9 --upper-bound 0xffffffffffffffff0040c62a2baca5b9
```

You will see only the votes against the proposal `ramusetest`.

##### Example (get all proposals a voter voted for):

The idea is to turn the `voter` into an integer, convert it to hexadecimal, and compute the lowest
possible key for `proposal_name` (lower bound) as well as the highest possible key for `proposal_name` (upper bound).

**Note** The hexadecimal values below are all in little-endian format, so high bytes are on the right side
and low bytes on the left side.

Here the steps to compute the lower/upper bounds for the table query:

 1. Convert `testusertest` EOS name to hexadecimal using `eosc tools name`.

   ```
   eosc tools names testusertest

   from \ to  hex               hex_be            name          uint64
   ---------  ---               ------            ----          ------
   name       90b1ca57619db1ca  cab19d6157cab190  testusertest  14605628107949519248
   ```

 1. Create the `lower_bound` key by prepending `0000000000000000` to the `hex` value shown
   above: `0x000000000000000090b1ca57619db1ca`

 1. Create the `upper_bound` key by prepending `ffffffffffffffff` to the `hex` value shown
   above: `0xffffffffffffffff90b1ca57619db1ca`.

Now that we have the lower and upper bound keys, simply perform your query:

```
eosc get table eosio.forum eosio.forum vote --index 3 --key-type i128 --lower-bound 0x000000000000000090b1ca57619db1ca --upper-bound 0xffffffffffffffff90b1ca57619db1ca
```

You will see only the proposals that voter `testusertest` voted for.

#### Proposal JSON Structure Guidelines

The `proposal_json` should be structured against the EOS Enhancement Proposal 4
([EEP-4](https://eeps.io/EEPS/eep-4)) which describes how the `proposal_json`
field should be structured based on a predefined set of proposal types.

While it's not strictly required to follow the guidelines in [EEP-4](https://eeps.io/EEPS/eep-4),
it's strongly suggested to do so as UI, vote tallies and related tools
use the guidelines in [EEP-4](https://eeps.io/EEPS/eep-4) to provide their
services.

If you decide to not follow the guidelines and instead create you own type(s),
it's highly encouraged to have a `type` field in your JSON string describing
your proposal type. Be sure that it does not collapse with the ones defined in
[EEP-4](https://eeps.io/EEPS/eep-4).

Of course, if you think your new type could be beneficial to the broader
community of EOS, you are invited to submit changes to [EEP-4](https://eeps.io/EEPS/eep-4)
via a GitHub pull request on the [EEP Repository](https://github.com/eoscanada/eeps).

#### General JSON Structure Guidelines

You can use any vocabulary you want when creating posts and votes, there
is no specification yet for the JSON of those actions. However, by following
some simple guidelines, you can simplify your life and the life of those
building UIs around these messages.

For all `json` prefixed or suffixed fields in `vote` and
`post`, the `type` field should determine a higher order protocol, and
determines what other sibling fields will be required.

##### In a `vote`'s `vote_json` field

* `type` is optional. Defaults to `simple` if not present.

###### `type` values

* `simple` is the same as no `type` at all. The value of the vote is
  the boolean `vote` field of the _action_.

##### In a `post`'s `json_metadata` field

* `type` is a required field to distinguish protocol.  See below for
  sample types

The following fields attempt to standardize the meaning of certain
keys. If you specify your own `type`, you can define whatever you
want.

* `title` is a title that will be shown above a message, often used in
  clickable headlines. Similar to a Reddit post's title.

* `tags` is a list of strings, prefixed or not with a `#`.

###### `type` values

* `chat`, which is a simple chat, pushing a message out.

* `eos-bps-roll-call`, this is used within EOS Block Producers calls
  to indicate they are present.

* `eos-bps-emergency`, once **3** block producers send a message of
  this type within an hour, all block producers can trigger a wake-up
  alarm within 1h. Do not abuse this message to avoid alert
  fatigue. ##### Example serious vulnerability requires mitigation, serious
  network issues, immediate action required, etc..

* `eos-bps-notify`, once **7** block producers send a message of this type
  within an hour, other block producers can trigger a notification to
  get their attention in the **next 24h**. ##### Example new ECAF order requires
  attention.

* `eos-arbitration-order`, BPs can watch for known Arbitration forums
  accounts, and alert themselves of required action.  Further fields
  could be defined like a link to the PDF format order; a reference to
  a ready-made `eosio.msig` transaction proposition; etc..

### License

MIT [See license file](./LICENSE.md)

### Credits

Original code and inspiration: Daniel Larimer
