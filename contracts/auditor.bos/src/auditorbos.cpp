#include <eosio/eosio.hpp>
#include <eosio/singleton.hpp>
#include <eosio/asset.hpp>
#include <eosio/transaction.hpp>

#include <eosio/multi_index.hpp>
#include <eosio/crypto.hpp>
#include <string>
#include "auditorbos.hpp"

#include "update_member_details.cpp"
#include "registering.cpp"
#include "voting.cpp"
#include "newtenure_components.cpp"
#include "external_observable_actions.cpp"
#include "config.cpp"

using namespace eosio;
using namespace std;

