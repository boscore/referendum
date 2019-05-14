#include <eosiolib/eosio.hpp>
#include <eosiolib/multi_index.hpp>
#include <eosiolib/singleton.hpp>
#include <eosiolib/time.hpp>

#include "external_types.hpp"

#define _STRINGIZE(x) #x
#define STRINGIZE(x) _STRINGIZE(x)

#ifdef TOKENCONTRACT
#define TOKEN_CONTRACT STRINGIZE(TOKENCONTRACT)
#endif

#ifndef TOKEN_CONTRACT
#define TOKEN_CONTRACT "eosio.token"
#endif

#ifndef TRANSFER_DELAY
#define TRANSFER_DELAY 60*60
#endif

const name AUDITORS_PERMISSION = "auditors"_n;

using namespace eosio;
using namespace std;

/**
 * - lockupasset (asset) -  The amount of assets that are locked up by each candidate applying for election.
 * - maxvotes (int default=5) - The maximum number of votes that each member can make for a candidate.
 * - numelected (int) -  Number of auditors to be elected for each election count.
 * - auditor_tenure (uint32 =  90 * 24 * 60 * 60) - Length of a period in seconds. Used for pay calculations if an early election is called and to trigger deferred `newtenure` calls.
 * - authaccount ( account= "auditor.bos") - account to have active auth set with all auditors on the newtenure.
 * - initial_vote_quorum_percent (uint32) - Amount of token value in votes required to trigger the initial set of auditors
 * - vote_quorum_percent (uint32) - Amount of token value in votes required to trigger the allow a new set of auditors to be set after the initial threshold has been achieved.
 * - auth_threshold_auditors (uint8) - Number of auditors required to approve the lowest level actions.
 * - lockup_release_time_delay (date) - The time before locked up stake can be released back to the candidate using the unstake action
 */
struct [[eosio::table("config"), eosio::contract("auditorbos")]] contr_config {
    // The amount of assets that are locked up by each candidate applying for election.
    asset lockupasset;

    // The maximum number of votes that each member can make for a candidate.
    uint8_t maxvotes = 3;

    // Number of auditors to be elected for each election count.
    uint8_t numelected = 5;

    // Length of a period in seconds.
    // - used for pay calculations if an eary election is called and to trigger deferred `newtenure` calls.
    uint32_t auditor_tenure = 90 * 24 * 60 * 60;

    // account to have active auth set with all all auditors on the newtenure.
    name authaccount = name{0};

    // Amount of token value in votes required to trigger the initial set of auditors
    uint32_t initial_vote_quorum_percent;

    // Amount of token value in votes required to trigger the allow a new set of auditors to be set after the initial threshold has been achieved.
    uint32_t vote_quorum_percent;

    // required number of auditors required to approve different levels of authenticated actions.
    uint8_t auth_threshold_auditors;

    // The time before locked up stake can be released back to the candidate using the unstake action
    uint32_t lockup_release_time_delay;

    EOSLIB_SERIALIZE(contr_config,
                    (lockupasset)
                    (maxvotes)
                    (numelected)
                    (auditor_tenure)
                    (authaccount)
                    (initial_vote_quorum_percent)
                    (vote_quorum_percent)
                    (auth_threshold_auditors)
                    (lockup_release_time_delay)
    )
};

typedef singleton<"config"_n, contr_config> configscontainer;

struct [[eosio::table("state"), eosio::contract("auditorbos")]] contr_state {
    uint32_t lastperiodtime = 0;
    int64_t total_weight_of_votes = 0;
    int64_t total_votes_on_candidates = 0;
    uint32_t number_active_candidates = 0;
    bool met_initial_votes_threshold = false;

    EOSLIB_SERIALIZE(contr_state, (lastperiodtime)
            (total_weight_of_votes)
            (total_votes_on_candidates)
            (number_active_candidates)
            (met_initial_votes_threshold)
    )
};

typedef singleton<"state"_n, contr_state> statecontainer;

// Utility to combine ids to help with indexing.
uint128_t combine_ids(const uint8_t &boolvalue, const uint64_t &longValue) {
    return (uint128_t{boolvalue} << 8) | longValue;
}

/**
 * - candidate_name (name) - Account name of the candidate (INDEX)
 * - is_active (int8) - Boolean indicating if the candidate is currently available for election. (INDEX)
 * - locked_tokens (asset) - An asset object representing the number of tokens locked when registering
 * - total_votes (uint64) - Updated tally of the number of votes cast to a candidate. This is updated and used as part of the `newtenure` calculations. It is updated every time there is a vote change or a change of token balance for a voter for this candidate to facilitate live voting stats.
 */
struct [[eosio::table("candidates"), eosio::contract("auditorbos")]] candidate {
    name candidate_name;
    asset locked_tokens;
    uint64_t total_votes;
    uint8_t is_active;
    time_point_sec auditor_end_time_stamp;

    uint64_t primary_key() const { return candidate_name.value; }

    uint64_t by_number_votes() const { return static_cast<uint64_t>(total_votes); }

    uint64_t by_votes_rank() const { return static_cast<uint64_t>(UINT64_MAX - total_votes); }

    EOSLIB_SERIALIZE(candidate,
                     (candidate_name)(locked_tokens)(total_votes)(is_active)(auditor_end_time_stamp))
};

typedef multi_index<"candidates"_n, candidate,
        indexed_by<"bycandidate"_n, const_mem_fun<candidate, uint64_t, &candidate::primary_key> >,
        indexed_by<"byvotes"_n, const_mem_fun<candidate, uint64_t, &candidate::by_number_votes> >,
        indexed_by<"byvotesrank"_n, const_mem_fun<candidate, uint64_t, &candidate::by_votes_rank> >
> candidates_table;

/**
 * - auditor_name (name) - Account name of the auditor (INDEX)
 * - total_votes - Tally of the number of votes cast to a auditor when they were elected in. This is updated as part of the `newtenure` action.
 */
struct [[eosio::table("auditors"), eosio::contract("auditorbos")]] auditor {
    name auditor_name;
    uint64_t total_votes;

    uint64_t primary_key() const { return auditor_name.value; }

    uint64_t by_votes_rank() const { return static_cast<uint64_t>(UINT64_MAX - total_votes); }

    EOSLIB_SERIALIZE(auditor,
                     (auditor_name)(total_votes))
};


struct [[eosio::table("bios"), eosio::contract("auditorbos")]] bios {
    name candidate_name;
    string bio;

    uint64_t primary_key() const { return candidate_name.value; }
    EOSLIB_SERIALIZE(bios, (candidate_name)(bio))
};

typedef multi_index<"bios"_n, bios > bios_table;


typedef multi_index<"auditors"_n, auditor,
        indexed_by<"byvotesrank"_n, const_mem_fun<auditor, uint64_t, &auditor::by_votes_rank> >
> auditors_table;

/**
 * - voter (account_name) - The account name of the voter (INDEX)
 * - candidates (account_name[]) - The candidates voted for, can supply up to the maximum number of votes (currently 5) - Can be configured via `updateconfig`
 */
struct [[eosio::table("votes"), eosio::contract("auditorbos")]] vote {
    name voter;
    name proxy;
    uint64_t weight;
    std::vector<name> candidates;

    uint64_t primary_key() const { return voter.value; }
    uint64_t by_proxy() const { return proxy.value; }


    EOSLIB_SERIALIZE(vote, (voter)(proxy)(weight)(candidates))
};

typedef eosio::multi_index<"votes"_n, vote,
        indexed_by<"byproxy"_n, const_mem_fun<vote, uint64_t, &vote::by_proxy> >
> votes_table;

struct [[eosio::table("pendingstake"), eosio::contract("auditorbos")]] tempstake {
    name sender;
    asset quantity;
    string memo;

    uint64_t primary_key() const { return sender.value; }

    EOSLIB_SERIALIZE(tempstake, (sender)(quantity)(memo))
};

typedef multi_index<"pendingstake"_n, tempstake> pendingstake_table_t;


class auditorbos : public contract {

private: // Variables used throughout the other actions.
    configscontainer config_singleton;
    statecontainer contract_state;
    candidates_table registered_candidates;
    votes_table votes_cast_by_members;
    bios_table candidate_bios;
    contr_state _currentState;

public:

    auditorbos( name s, name code, datastream<const char*> ds )
        :contract(s,code,ds),
            registered_candidates(_self, _self.value),
            votes_cast_by_members(_self, _self.value),
            candidate_bios(_self, _self.value),
            config_singleton(_self, _self.value),
            contract_state(_self, _self.value) {

        _currentState = contract_state.get_or_default(contr_state());
    }

    ~auditorbos() {
        contract_state.set(_currentState, _self); // This should not run during a contract_state migration since it will prevent changing the schema with data saved between runs.
    }

    /**
     * ### updateconfig
     *
     * Updates the contract configuration parameters to allow changes without needing to redeploy the source code.
     *
     * #### Message
     *
     * updateconfig(<params>)
     *
     * This action asserts:
     *
     * - the message has the permission of the contract account.
     * - the supplied asset symbol matches the current lockup symbol if it has been previously set or that there have been no 	.
     *
     * The parameters are:
     *
     * - lockupasset(uint8_t) : defines the asset and amount required for a user to register as a candidate. This is the amount that will be locked up until the user calls `withdrawcand` in order to get the asset returned to them. If there are currently already registered candidates in the contract this cannot be changed to a different asset type because of introduced complexity of handling the staked amounts.
     * - maxvotes(asset) : Defines the maximum number of candidates a user can vote for at any given time.
     * - numelected(uint16_t) : The number of candidates to elect for auditors. This is used for the payment amount to auditors for median amount.
     * - auditor_tenure(uint32_t) : The length of a period in seconds. This is used for the scheduling of the deferred `newtenure` actions at the end of processing the current one. Also is used as part of the partial payment to auditors in the case of an elected auditor resigning which would also trigger a `newtenure` action.
     * - authaccount(name) : The managing account that controls the BOS auditor permission.
     * - initial_vote_quorum_percent (uint32) : The percent of voters required to activate BOS for the first election period.
     * - vote_quorum_percent (uint32) : The percent of voters required to continue BOS for the following election periods after the first one has activated BOS.
     * - auth_threshold_auditors (uint8) : The number of auditors required to approve an action in the low permission category ( ordinary action such as a worker proposal).
     */
    ACTION updateconfig(contr_config newconfig);

    /**
     * Action to listen to from the associated token contract to ensure registering should be allowed.
     *
     * @param from The account to observe as the source of funds for a transfer
     * @param to The account to observe as the destination of funds for a transfer
     * @param quantity
     * @param memo A string to attach to a transaction. For staking this string should match the name of the running contract eg "auditor.bos". Otherwise it will be regarded only as a generic transfer to the account.
     * This action is intended only to observe transfers that are run by the associated token contract for the purpose of tracking the moving weights of votes if either the `from` or `to` in the transfer have active votes. It is not included in the ABI to prevent it from being called from outside the chain.
     */
    void transfer(name from,
                  name to,
                  asset quantity,
                  string memo);


    /**
     * This action is used to nominate a candidate for auditor elections.
     * It must be authorised by the candidate and the candidate must be an active member of BOS, having agreed to the latest constitution.
     * The candidate must have transferred a quantity of tokens (determined by a config setting - `lockupasset`) to the contract for staking before this action is executed. This could have been from a recent transfer with the contract name in the memo or from a previous time when this account had nominated, as long as the candidate had never `unstake`d those tokens.
     *
     * ### Assertions:
     * - The account performing the action is authorised.
     * - The candidate is not already a nominated candidate.
     * - The requested pay amount is not more than the config max amount
     * - The requested pay symbol type is the same from config max amount ( The contract supports only one token symbol for payment)
     * - The candidate is currently a member or has agreed to the latest constitution.
     * - The candidate has transferred sufficient funds for staking if they are a new candidate.
     * - The candidate has enough staked if they are re-nominating as a candidate and the required stake has changed since they last nominated.
     *
     * @param cand - The account id for the candidate nominating.
     *
     *
     * ### Post Condition:
     * The candidate should be present in the candidates table and be set to active. If they are a returning candidate they should be set to active again. The `locked_tokens` value should reflect the total of the tokens they have transferred to the contract for staking. The number of active candidates in the contract will incremented.
     */
    ACTION nominatecand(name cand);

    /**
     * This action is used to withdraw a candidate from being active for auditor elections.
     *
     * ### Assertions:
     * - The account performing the action is authorised.
     * - The candidate is already a nominated candidate.
     *
     * @param cand - The account id for the candidate nominating.
     *
     *
     * ### Post Condition:
     * The candidate should still be present in the candidates table and be set to inactive. If the were recently an elected auditor there may be a time delay on when they can unstake their tokens from the contract. If not they will be able to unstake their tokens immediately using the unstake action.
     */
    ACTION withdrawcand(name cand);

    /**
     * This action is used to remove a candidate from being a candidate for auditor elections.
     *
     * ### Assertions:
     * - The action is authorised by the mid level permission the auth account for the contract.
     * - The candidate is already a nominated candidate.
     *
     * @param cand - The account id for the candidate nominating.
     * @param lockupStake - if true the stake will be locked up for a time period as set by the contract config - `lockup_release_time_delay`
     *
     *
     * ### Post Condition:
     * The candidate should still be present in the candidates table and be set to inactive. If the `lockupstake` parameter is true the stake will be locked until the time delay has passed. If not the candidate will be able to unstake their tokens immediately using the unstake action to have them returned.
     */
    ACTION firecand(name cand, bool lockupStake);

    /**
     * This action is used to resign as a auditor.
     *
     * ### Assertions:
     * - The `auditor` account performing the action is authorised to do so.
     * - The `auditor` account is currently an elected auditor.
     *
     * @param auditor - The account id for the candidate nominating.
     *
     *
     * ### Post Condition:
     * The auditor will be removed from the active auditors and should still be present in the candidates
     * table but will be set to inactive. Their staked tokens will be locked up for the time delay added from
     * the moment this action was called so they will not able to unstake until that time has passed.
     *
     * A replacement auditor will selected from the candidates to fill the missing place (based on vote ranking)
     * then the auths for the controlling BOS auth account will be set for the auditor board.
     */
    ACTION resign(name auditor);

    /**
     * This action is used to remove a auditor.
     *
     * ### Assertions:
     * - The action is authorised by the mid level of the auth account (currently elected auditor board).
     * - The `auditor` account is currently an elected auditor.
     *
     * @param auditor - The account id for the candidate nominating.
     *
     *
     * ### Post Condition:
     * The auditor will be removed from the active auditors and should still be present in the candidates table but will be set to inactive.
     * Their staked tokens will be locked up for the time delay added from the moment this action was called so they will not able to unstake until
     * that time has passed. A replacement auditor will selected from the candidates to fill the missing place (based on vote ranking)
     * then the auths for the controlling BOS auth account will be set for the auditor board.
     */
    ACTION fireauditor(name auditor);

    /**
     * This action is used to update the bio for a candidate.
     *
     * ### Assertions:
     * - The `cand` account performing the action is authorised to do so.
     * - The string in the bio field is less than 256 characters.
     *
     * @param cand - The account id for the candidate nominating.
     * @param bio - A string of bio data that will be passed through the contract.
     *
     *
     * ### Post Condition:
     * Nothing from this action is stored on the blockchain. It is only intended to ensure authentication of changing the bio which will be stored off chain.
     */
    ACTION updatebio(name cand, std::string bio);

    /**
     * This action is to facilitate voting for candidates to become auditors of BOS.
     * Each member will be able to vote a configurable number of auditors set by the contract configuration.
     * When a voter calls this action either a new vote will be recorded or the existing vote for that voter will be modified.
     * If an empty array of candidates is passed to the action an existing vote for that voter will be removed.
     *
     * ### Assertions:
     * - The voter account performing the action is authorised to do so.
     * - The voter account performing has agreed to the latest member terms for BOS.
     * - The number of candidates in the newvotes vector is not greater than the number of allowed votes per voter as set by the contract config.
     * - Ensure there are no duplicate candidates in the voting vector.
     * - Ensure all the candidates in the vector are registered and active candidates.
     *
     * @param voter - The account id for the voter account.
     * @param newvotes - A vector of account ids for the candidate that the voter is voting for.
     *
     * ### Post Condition:
     * An active vote record for the voter will have been created or modified to reflect the newvotes. Each of the candidates will have their total_votes amount updated to reflect the delta in voter's token balance. Eg. If a voter has 1000 tokens and votes for 5 candidates, each of those candidates will have their total_votes value increased by 1000. Then if they change their votes to now vote 2 different candidates while keeping the other 3 the same there would be a change of -1000 for 2 old candidates +1000 for 2 new candidates and the other 3 will remain unchanged.
     */
    ACTION voteauditor(name voter, std::vector<name> newvotes);

    /**
     * Refresh vote since `eosio` does not notify this contract
     *
     * @param voter - The account id for the voter account.
     */
    ACTION refreshvote(name voter);

    /**
     * ### newtenure
     *
     * This action is to be run to end and begin each period in BOS life cycle. It performs multiple tasks for BOS including:
     *
     * - Allocate auditors from the candidates tables based on those with most votes at the moment this action is run. -- This action removes and selects a full set of auditors each time it is successfully run selected from the candidates with the most votes weight. If there are not enough eligible candidates to satisfy BOS config numbers the action adds the highest voted candidates as auditors as long their votes weight is greater than 0. At this time the held stake for the departing auditors is set to have a time delayed lockup to prevent the funds from releasing too soon after each auditor has been in office.
     * - Distribute pay for the existing auditors based on the configs into the pending pay table so it can be claimed by individual candidates. -- The pay is distributed as determined by the median pay of the currently elected auditors. Therefore all elected auditors receive the same pay amount.
     * - Set BOS auths for the intended controlling accounts based on the configs thresholds with the newly elected auditors. This action asserts unless the following conditions have been met:
     * - The action cannot be called multiple times within the period since the last time it was previously run successfully. This minimum time between allowed calls is configured by the period length parameter in contract configs.
     * - To run for the first time a minimum threshold of voter engagement must be satisfied. This is configured by the `initial_vote_quorum_percent` field in the contract config with the percentage calculated from the amount of registered votes cast by voters against the max supply of tokens for BOS's primary currency.
     * - After the initial vote quorum percent has been reached subsequent calls to this action will require a minimum of `vote_quorum_percent` to vote for the votes to be considered sufficient to trigger a new period with new auditors.
     *
     * ##### Parameters:
     *
     * message - a string that is used to log a message in the chain history logs. It serves no function in the contract logic.
     */
    ACTION newtenure(std::string message);

    /**
     * This action is used to unstake a candidates tokens and have them transferred to their account.
     *
     * ### Assertions:
     * - The candidate was a nominated candidate at some point in the passed.
     * - The candidate is not already a nominated candidate.
     * - The tokens held under candidate's account are not currently locked in a time delay.
     *
     * @param cand - The account id for the candidate nominating.
     *
     *
     * ### Post Condition:
     * The candidate should still be present in the candidates table and should be still set to inactive. The candidates tokens will be transferred back to their account and their `locked_tokens` value will be reduced to 0.
     */
    ACTION unstake(name cand);


private: // Private helper methods used by other actions.

    contr_config configs();

    void updateVoteWeight(name auditor, int64_t weight);

    void updateVoteWeights(const vector<name> &votes, int64_t vote_weight);

    void modifyVoteWeights(name voter, vector<name> newVotes);

    void assertPeriodTime();

    void setAuditorAuths();

    void removeAuditor(name auditor);

    void removeCandidate(name auditor, bool lockupStake);

    void allocateAuditors(bool early_election);

};
