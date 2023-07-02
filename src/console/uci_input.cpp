#include "uci_input.hpp"

#include "console/uci_output.hpp"
#include "engine/Engine.hpp"
#include "engine/Settings.hpp"
#include "engine/UCISetting.hpp"
#include "logging/logging.hpp"
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
			Move move(args.at(i));

			// Decode move type (if not a promotion move that is encoded in the string)
			// This will be needed elsewhere, so should be extracted
			if (move.get_type() == MoveType::Quiet)
			{
				const Square from_square = move.get_from_square();
				const Square to_square = move.get_to_square();
				const Piece piece = position.get_piece(from_square);
				const Color to_color = position.get_color(to_square);
				const uint8_t from_file = from_square.get_file();
				const uint8_t to_file = to_square.get_file();
				const uint8_t file_diff = (from_file > to_file) ? (from_file - to_file) : (to_file - from_file);

				// Castling
				if (piece == Piece::King && file_diff > 1)
				{
					if (from_file > to_file)
					{
						move.set_type(MoveType::QueenCastle);
					}
					if (from_file < to_file)
					{
						move.set_type(MoveType::KingCastle);
					}
				}

				// En Passant
				if (piece == Piece::Pawn && file_diff == 1 && to_color == Color::Empty)
				{
					move.set_type(MoveType::EnPassant);
				}
			}

			position.make_move(move);
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
	// This parsing is kind of ass

	std::string id_string = args.at(1);

	size_t value_string_index = 2;

	for (; value_string_index < args.size() && args.at(value_string_index) != "value"; value_string_index++)
	{
		id_string += " " + args.at(value_string_index);
	}

	SettingID id = engine_settings.find_id_by_name(id_string);

	std::string value_string;

	if (value_string_index < args.size() - 1)
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

		case SettingID::LogFilepath:
		{
			set_log_filepath(value_string);
			break;
		}

		case SettingID::Hash:
		{
			// Not handling hash right now
			break;
		}

		default:
		{
			std::printf("Unhandled setting (%s)!\n", id_string.c_str());
			break;
		}
	}
}