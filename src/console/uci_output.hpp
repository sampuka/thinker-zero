#ifndef CONSOLE_UCI_OUTPUT_HPP
#define CONSOLE_UCI_OUTPUT_HPP

#include "types/Move.hpp"

void uci_readyok();

void uci_uciok();

void uci_bestmove(const Move& move);

#endif // CONSOLE_UCI_OUTPUT_HPP
