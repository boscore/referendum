#include <eosiolib/eosio.hpp>
#include <eosiolib/asset.hpp>

using namespace eosio;
using namespace std;

struct account {
    asset balance;

    uint64_t primary_key() const { return balance.symbol.code().raw(); }
};

typedef eosio::multi_index<"accounts"_n, account> accounts;

struct currency_stats {
    eosio::asset supply;
    eosio::asset max_supply;
    name issuer;

    uint64_t primary_key() const { return supply.symbol.code().raw(); }
};

typedef eosio::multi_index<"stat"_n, currency_stats> stats;

/**
 * Every user 'from' has a scope/table that uses every receipient 'to' as the primary key.
 */
struct delegated_bandwidth {
    name          from;
    name          to;
    asset         net_weight;
    asset         cpu_weight;

    uint64_t  primary_key()const { return to.value; }
};

typedef eosio::multi_index< "delband"_n, delegated_bandwidth > del_bandwidth_table;

//Authority Structs
namespace eosiosystem {

    struct key_weight {
        eosio::public_key key;
        uint16_t weight;

        // explicit serialization macro is not necessary, used here only to improve compilation time
        EOSLIB_SERIALIZE(key_weight, (key)(weight))
    };

    struct permission_level_weight {
        permission_level permission;
        uint16_t weight;

        // explicit serialization macro is not necessary, used here only to improve compilation time
        EOSLIB_SERIALIZE(permission_level_weight, (permission)(weight))
    };

    struct wait_weight {
        uint32_t wait_sec;
        uint16_t weight;

        // explicit serialization macro is not necessary, used here only to improve compilation time
        EOSLIB_SERIALIZE(wait_weight, (wait_sec)(weight))
    };

    struct authority {

        uint32_t threshold;
        vector<key_weight> keys;
        vector<permission_level_weight> accounts;
        vector<wait_weight> waits;

        EOSLIB_SERIALIZE(authority, (threshold)(keys)(accounts)(waits))
    };
}
