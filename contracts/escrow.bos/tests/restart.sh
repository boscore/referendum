#!/bin/sh

nodeos -e -p eosio \
	--plugin eosio::producer_plugin \
	--plugin eosio::chain_api_plugin \
	--plugin eosio::http_plugin \
	--plugin eosio::history_plugin \
	--plugin eosio::history_api_plugin \
	--access-control-allow-origin="*" \
	--contracts-console \
    --delete-all-blocks --verbose-http-errors
