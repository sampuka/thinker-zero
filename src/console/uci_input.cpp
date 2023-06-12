#include "uci_input.hpp"

#include "console/uci_output.hpp"
#include "engine/Engine.hpp"
#include "position/PositionString.hpp"

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

	Position position = position_string.get_position();
	if (args.size() >= 2 && args.at(1) == "moves")
	{
		for (size_t i = 2; i < args.size(); i++)
		{
			position.make_move(Move(args.at(i)));
		}
	}

	engine.set_position(position);
}

void uci_go(const std::vector<std::string>& args)
{
	(void)args;

	engine.go();
}