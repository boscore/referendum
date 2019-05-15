#!/usr/bin/env bash

cd src
eosio-cpp forum.cpp -o ../forum.wasm -abigen -I ../include -R ../resources
