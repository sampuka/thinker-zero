#!/bin/sh

cutechess-cli \
    -engine cmd=../build/abpq_engine \
    -engine cmd=../build/minimax_engine \
    -engine cmd=../build/random_engine \
    -engine cmd=/home/sampuka/ChessArena/Engines/Stockfish/stockfish_10_x64_modern_official option."Skill Level"=1\
    -each proto=uci tc=40/60 -rounds 1
