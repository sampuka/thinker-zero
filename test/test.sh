#!/bin/sh

cutechess-cli \
    -engine cmd=../build/abp_engine \
    -engine cmd=../build/minimax_engine \
    -engine cmd=../build/random_engine \
    -each proto=uci tc=40/60 -rounds 3
