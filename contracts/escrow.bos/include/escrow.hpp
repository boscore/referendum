#include <eosio/eosio.hpp>
#include <eosio/asset.hpp>
#include <eosio/time.hpp>
#include <eosio/transaction.hpp>

#include <string>
#include <optional>

using eosio::const_mem_fun;
using eosio::indexed_by;
using eosio::multi_index;
using eosio::extended_asset;
using eosio::check;
using eosio::datastream;
using eosio::contract;
using eosio::print;
using eosio::name;
using eosio::asset;
using eosio::symbol;
using eosio::time_point_sec;
using eosio::current_time_point;
using std::vector;
using std::function;
using std::string;

class [[eosio::contract("escrow")]] escrow : public eosio::contract {
    public:

        escrow(name s, name code, datastream<const char *> ds)
                : contract(s, code, ds),
                  escrows(_self, _self.value) {
            sending_code = name{code};
        }

        ~escrow();

        [[eosio::on_notify("eosio.token::transfer")]]
        void transfer(name from, name to, asset quantity, string memo);

        [[eosio::action]]
        void init(
            const name           sender,
            const name           receiver,
            const name           approver,
            const name           escrow_name,
            const time_point_sec expires_at,
            const string         memo
        );

        [[eosio::action]]
        void approve(const name escrow_name, const name approver);

        [[eosio::action]]
        void unapprove(const name escrow_name, const name unapprover);

        [[eosio::action]]
        void claim(const name escrow_name);

        [[eosio::action]]
        void refund(const name escrow_name);

        [[eosio::action]]
        void cancel(const name escrow_name);

        [[eosio::action]]
        void extend(const name escrow_name, const time_point_sec expires_at);

        [[eosio::action]]
        void close(const name escrow_name);

        [[eosio::action]]
        void lock(const name escrow_name, const bool locked);

        [[eosio::action]]
        void clean();

    private:
        // 6 months in seconds (Computatio: 6 months * average days per month * 24 hours * 60 minutes * 60 seconds)
        constexpr static uint32_t SIX_MONTHS_IN_SECONDS = (uint32_t) (6 * (365.25 / 12) * 24 * 60 * 60);

        struct [[eosio::table]] escrow_row {
            name            escrow_name;
            name            sender;
            name            receiver;
            name            approver;
            vector<name>    approvals;
            extended_asset  ext_asset;
            string          memo;
            time_point_sec  created_at;
            time_point_sec  expires_at;
            bool            locked = false;

            auto            primary_key() const { return escrow_name.value; }
            uint64_t        by_sender() const { return sender.value; }
            bool            is_expired() const { return time_point_sec(current_time_point()) > expires_at; }
        };

        typedef multi_index<"escrows"_n, escrow_row,
            indexed_by<"bysender"_n, const_mem_fun<escrow_row, uint64_t, &escrow_row::by_sender> >
        > escrows_table;

        escrows_table escrows;
        name sending_code;
};
