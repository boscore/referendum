#!/usr/bin/env bash

cd src
eosio-cpp auditorbos.cpp -o ../auditorbos.wasm -abigen -I ../include -I "."
