#!/bin/sh

cutechess-cli \
    -engine cmd=../build/abp_engine \
    -engine cmd=../build/minimax_engine \
    -engine cmd=/home/sampuka/ChessArena/Engines/Stockfish/stockfish_10_x64_modern_official option."Skill Level"=1 name="SF1"\
    -engine cmd=/home/sampuka/ChessArena/Engines/Stockfish/stockfish_10_x64_modern_official option."Skill Level"=2 name="SF2"\
    -engine cmd=/home/sampuka/ChessArena/Engines/Stockfish/stockfish_10_x64_modern_official option."Skill Level"=3 name="SF3"\
    -each proto=uci tc=0/120+2 -rounds 16
