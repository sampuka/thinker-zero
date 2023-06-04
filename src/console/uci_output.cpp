#include "uci_output.hpp"

void uci_readyok()
{
    std::printf("readyok\n");
}

void uci_uciok()
{
    std::printf(
        "id name Thinker-zero Chess Engine\n"
        "id author Mathias Ebbensgaard Jensen\n"
        );

    // Send supported settings

    std::printf("uciok\n");
}

void uci_bestmove(const Move& move)
{
    uint8_t from = move.get_from_square().get_data();
    uint8_t to = move.get_to_square().get_data();
    
    std::string str;
    str += (from % 8) + 'a';
    str += (from / 8) + '1';
    str += (to % 8) + 'a';
    str += (to / 8) + '1';

    std::printf("bestmove %s\n", str.c_str());
}