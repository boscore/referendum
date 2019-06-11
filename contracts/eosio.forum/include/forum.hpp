#pragma once

#include <algorithm>
#include <string>

#include <eosio/eosio.hpp>
#include <eosio/time.hpp>
#include <eosio/system.hpp>

using eosio::check;
using eosio::const_mem_fun;
using eosio::current_time_point;
using eosio::datastream;
using eosio::indexed_by;
using eosio::name;
using eosio::time_point_sec;
using std::function;
using std::string;

class [[eosio::contract("forum")]] forum : public eosio::contract {
    public:
        forum(name receiver, name code, datastream<const char*> ds)
        :eosio::contract(receiver, code, ds)
        {}
        [[eosio::action]]
        void propose(
            const name proposer,
            const name proposal_name,
            const string& title,
            const string& proposal_json
        );

        [[eosio::action]]
        void vote(
            const name voter,
            const name proposal_name,
            uint8_t vote,
            const string& vote_json
        );

        [[eosio::action]]
        void unvote(const name voter, const name proposal_name);

        [[eosio::action]]
        void post(
            const name poster,
            const string& post_uuid,
            const string& content,
            const name reply_to_poster,
            const string& reply_to_post_uuid,
            const bool certify,
            const string& json_metadata
        );

        [[eosio::action]]
        void unpost(const name poster, const string& post_uuid);

        [[eosio::action]]
        void status(const name account, const string& content);

        [[eosio::action]]
        void cancel(
            const name proposer,
            const name proposal_name
        );

    private:
        static uint128_t compute_by_proposal_key(const name proposal_name, const name voter) {
            return ((uint128_t) proposal_name.value) << 64 | voter.value;
        }

        static uint128_t compute_by_voter_key(const name proposal_name, const name voter) {
            return ((uint128_t) voter.value) << 64 | proposal_name.value;
        }

        struct [[eosio::table]] proposal_row {
            name                  proposal_name;
            name                  proposer;
            string                title;
            string                proposal_json;
            time_point_sec        created_at;

            auto primary_key()const { return proposal_name.value; }
            uint64_t by_proposer() const { return proposer.value; }
        };
        typedef eosio::multi_index<
            "proposal"_n, proposal_row,
            indexed_by<"byproposer"_n, const_mem_fun<proposal_row, uint64_t, &proposal_row::by_proposer>>
        > proposals;

        struct [[eosio::table]] vote_row {
            uint64_t               id;
            name                   proposal_name;
            name                   voter;
            uint8_t                vote;
            string                 vote_json;
            time_point_sec         updated_at;

            auto primary_key() const { return id; }
            uint128_t by_proposal() const { return forum::compute_by_proposal_key(proposal_name, voter); }
            uint128_t by_voter() const { return forum::compute_by_voter_key(proposal_name, voter); }
        };
        typedef eosio::multi_index<
            "vote"_n, vote_row,
            indexed_by<"byproposal"_n, const_mem_fun<vote_row, uint128_t, &vote_row::by_proposal>>,
            indexed_by<"byvoter"_n, const_mem_fun<vote_row, uint128_t, &vote_row::by_voter>>
        > votes;

        struct [[eosio::table]] status_row {
            name                 account;
            string               content;
            time_point_sec       updated_at;

            auto primary_key() const { return account.value; }
        };
        typedef eosio::multi_index<"status"_n, status_row> statuses;

        void update_status(
            statuses& status_table,
            const name account,
            const function<void(status_row&)> updater
        );

        void update_vote(
            votes& vote_table,
            const name proposal_name,
            const name voter,
            const function<void(vote_row&)> updater
        );

        // Do not use directly, use the VALIDATE_JSON macro instead!
        void validate_json(
            const string& payload,
            size_t max_size,
            const char* not_object_message,
            const char* over_size_message
        );
};
