#!/bin/sh

cutechess-cli \
    -engine cmd=../build/abp_engine \
    -engine cmd=../build/abpq_engine \
    -each proto=uci tc=0/600+5 -rounds 10

cutechess-cli \
    -engine cmd=../build/abp_engine \
    -engine cmd=../build/abpq_engine \
    -each proto=uci tc=40/120+1 -rounds 10

cutechess-cli \
    -engine cmd=../build/abp_engine \
    -engine cmd=../build/abpq_engine \
    -each proto=uci tc=40/30+1 -rounds 10
