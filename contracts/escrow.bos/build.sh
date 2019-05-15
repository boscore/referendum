#!/usr/bin/env bash

cd src
eosio-cpp escrow.cpp -o ../escrow.wasm -abigen -I ../include -R ../resources
