#include <eosio/eosio.hpp>
#include <eosio/asset.hpp>

using namespace eosio;
using namespace std;

//Authority Structs
namespace eosiosystem {

    struct key_weight {
        eosio::public_key key;
        uint16_t weight;
    };

    struct permission_level_weight {
        permission_level permission;
        uint16_t weight;
    };

    struct wait_weight {
        uint32_t wait_sec;
        uint16_t weight;
    };

    struct authority {
        uint32_t threshold;
        vector<key_weight> keys;
        vector<permission_level_weight> accounts;
        vector<wait_weight> waits;
    };
}
