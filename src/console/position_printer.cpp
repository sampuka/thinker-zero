#include "position_printer.hpp"

#include <cstdio>

char get_piece_as_char(Piece piece)
{
	char c;

	switch (piece)
	{
		case Piece::Pawn:
		{
			c = 'P';
			break;
		}
		case Piece::Knight:
		{
			c = 'N';
			break;
		}
		case Piece::Bishop:
		{
			c = 'B';
			break;
		}
		case Piece::Rook:
		{
			c = 'R';
			break;
		}
		case Piece::Queen:
		{
			c = 'Q';
			break;
		}
		case Piece::King:
		{
			c = 'K';
			break;
		}
		default:
		{
			c = '.';
		}
	}

	return c;
}

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

			char c = get_piece_as_char(piece);

			if (color == Color::Black)
			{
				c -= 'A' - 'a';  // Display black pieces as lowercase
			}

			str += c;
		}

		str += "║\n";
	}

	str += "╚════════╝\n";

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
