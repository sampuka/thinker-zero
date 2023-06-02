#include "uci_commands.hpp"

#include "engine/Engine.hpp"
#include "types/PositionString.hpp"

#include <cstdio>

void uci_start()
{
    std::printf(
        "id name Thinker-zero Chess Engine\n"
        "id author Mathias Ebbensgaard Jensen\n"
        );

    // Send supported settings

    std::printf("uciok\n");
}

void uci_isready()
{
    std::printf("readyok\n");
}

void uci_ucinewgame()
{
    engine.new_game();
}

void uci_position(const std::vector<std::string>& args)
{
    PositionString position_string(args.at(0));

    std::vector<Move> movelist;
    for (size_t i = 1; i < args.size(); i++)
    {
        movelist.emplace_back(args.at(i));
    }

    engine.set_position(position_string.get_position());

    for (const Move& move : movelist)
    {
        engine.perform_move(move);
    }
}

void uci_go(const std::vector<std::string>& args)
{
    (void)args;

    std::printf("bestmove e7e5\n");
}