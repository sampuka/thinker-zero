#include "position_printer.hpp"

#include "types/conversions.hpp"

#include <cstdio>

std::string format_position_to_string(const Position& position)
{
	std::string str;
	str += "╔════════╗\n";

	for (uint8_t rank = RANK_8; rank >= RANK_1; rank--)
	{
		str += "║";

		for (uint8_t file = FILE_A; file <= FILE_H; file++)
		{
			Square sq(file, rank);

			Piece piece = position.get_piece(sq);
			Color color = position.get_color(sq);

			char c = convert_piece_to_char(piece);

			if (color == Color::White)
			{
				c += 'A' - 'a';  // Display black pieces as lowercase
			}

			str += c;
		}

		str += "║\n";
	}

	str += "╚════════╝\n";

	str += "Castling rights: ";

	if (position.may_white_queenside_castle())
	{
		str += "Q";
	}
	if (position.may_white_kingside_castle())
	{
		str += "K";
	}
	if (position.may_black_queenside_castle())
	{
		str += "q";
	}
	if (position.may_black_kingside_castle())
	{
		str += "k";
	}

	str += "\n";

	if (position.get_player() == Color::White)
	{
		str += "White";
	}
	else
	{
		str += "Black";
	}

	str += " to play\n";

	return str;
}
