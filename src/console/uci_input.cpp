#include "uci_input.hpp"

#include "console/uci_output.hpp"
#include "engine/Engine.hpp"
#include "engine/Settings.hpp"
#include "engine/UCISetting.hpp"
#include "position/PositionString.hpp"
#include "util/string_utils.hpp"

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
	if (args.size() >= 1 && args.at(0) == "perft")
	{
		if (args.size() != 2)
		{
			std::printf("Usage: 'go perft <depth>'\n");
			return;
		}

		engine.perft(std::stoi(args.at(1)));
	}
	else
	{
		engine.go();
	}
}

void uci_setoption(const std::vector<std::string>& args)
{
	std::string id_string = args.at(1);

	size_t value_string_index = 2;

	for (; value_string_index < args.size() && args.at(value_string_index) != "value"; value_string_index++)
	{
		id_string += " " + args.at(value_string_index);
	}

	SettingID id = engine_settings.find_id_by_name(id_string);

	std::string value_string;

	if (value_string_index != args.size() - 1)
	{
		value_string_index++;
		value_string += args.at(value_string_index);
		value_string_index++;
	}

	for (; value_string_index < args.size(); value_string_index++)
	{
		value_string += " " + args.at(value_string_index);
	}

	switch (id)
	{
		case SettingID::RandomMovesOnly:
		{
			if (string_compare(value_string, "true"))
			{
				engine_settings.set_random_moves_only(true);
			}
			else if (string_compare(value_string, "false"))
			{
				engine_settings.set_random_moves_only(false);
			}
			break;
		}

		case SettingID::MaxSearchDepth:
		{
			engine_settings.set_max_search_depth(std::stoi(value_string));
			break;
		}

		default:
		{
			std::printf("Unhandled setting (%s)!\n", id_string.c_str());
			break;
		}
	}
}