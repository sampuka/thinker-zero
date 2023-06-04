#include "uci_input.hpp"

#include "console/uci_output.hpp"
#include "engine/Engine.hpp"
#include "types/PositionString.hpp"

#include <cstdio>

void uci_start()
{
    uci_uciok();
}

void uci_isready()
{
    uci_readyok();
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

    Position position = position_string.get_position();

    for (const Move& move : movelist)
    {
        (void)move;
        position.make_move(move);
    }

    engine.set_position(position);
}

void uci_go(const std::vector<std::string>& args)
{
    (void)args;

    engine.go();
}