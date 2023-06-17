#include "just_material.hpp"

namespace evaluation_jm
{
int get_piece_value(const Piece& piece)
{
	switch (piece)
	{
		case Piece::Pawn:
			return 1;
		case Piece::Knight:
			return 3;
		case Piece::Bishop:
			return 3;
		case Piece::Rook:
			return 5;
		case Piece::Queen:
			return 9;
		case Piece::King:
			// This should properly not be used.
			return 200;
		default:
			// Assumed to be Piece::Empty.
			return 0;
	}
	return 0;
}

int evaluate_board(const Position& position)
{
	int score = 0;
	for (uint8_t i = 0; i < 64; i++)
	{
		Square square(i);
		Piece piece_on_square = position.get_piece(square);

		// Checking if color is white and setting the sign to positive. If not white assumed it is black and sign is set to negative. If the color is empty then it does not matter what the sign is
		// since piece value will be zero.
		const int color_sign = (position.get_color(square) == Color::White) ? 1 : -1;
		score += color_sign * get_piece_value(piece_on_square);
	}
	return score;
}
}  // namespace evaluation_jm