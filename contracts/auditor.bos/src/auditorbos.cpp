#include <eosio/eosio.hpp>
#include <eosio/singleton.hpp>
#include <eosio/asset.hpp>
#include <eosio/transaction.hpp>

#include <eosio/multi_index.hpp>
#include <eosio/crypto.hpp>
#include <string>
#include "auditorbos.hpp"

#include "update_bio.cpp"
#include "registering.cpp"
#include "voting.cpp"
#include "newtenure.cpp"
#include "on_transfer.cpp"
#include "config.cpp"
#include "clean.cpp"

using namespace eosio;
using namespace std;

