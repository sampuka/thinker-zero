#!/bin/sh

cutechess-cli \
    -engine cmd=../build/abpq_engine \
    -engine cmd=../build/minimax_engine \
    -engine cmd=../build/random_engine \
    -engine cmd=/home/sampuka/ChessArena/Engines/Stockfish/stockfish_10_x64_modern_official option."Skill Level"=1 name="SF1"\
    -engine cmd=/home/sampuka/ChessArena/Engines/Stockfish/stockfish_10_x64_modern_official option."Skill Level"=2 name="SF2"\
    -engine cmd=/home/sampuka/ChessArena/Engines/Stockfish/stockfish_10_x64_modern_official option."Skill Level"=3 name="SF3"\
    -engine cmd=/home/sampuka/ChessArena/Engines/Stockfish/stockfish_10_x64_modern_official option."Skill Level"=4 name="SF4"\
    -each proto=uci tc=0/20+1 -rounds 2
