#include "simplified_evaluation_function.hpp"

#include "types/Piece.hpp"

namespace evaluation_sef
{
int get_piece_value(const Piece& piece)
{
	switch (piece)
	{
		case Piece::Pawn:
			return 100;
		case Piece::Knight:
			return 320;
		case Piece::Bishop:
			return 330;
		case Piece::Rook:
			return 500;
		case Piece::Queen:
			return 900;
		case Piece::King:
			// This should properly not be used.
			return 20000;
		default:
			// Assumed to be Piece::Empty.
			return 0;
	}
	return 0;
}

int get_pst_value(const Piece& piece, const int pst_index)
{
	switch (piece)
	{
		case Piece::Pawn:
			return pst_pawn[pst_index];
		case Piece::Knight:
			return pst_knight[pst_index];
		case Piece::Bishop:
			return pst_bishop[pst_index];
		case Piece::Rook:
			return pst_rook[pst_index];
		case Piece::Queen:
			return pst_queen[pst_index];
		case Piece::King:
			// This should properly not be used.
			return pst_king[pst_index];
		default:
			// Assumed to be Piece::Empty.
			return 0;
	}
	return 0;
}

int evaluate_board(const Position& position)
{
	float score = 0;
	for (uint8_t i = 0; i < 64; i++)
	{
		Square square(i);
		Piece piece_on_square = position.get_piece(square);
		if (piece_on_square == Piece::Empty)
		{
			// We don't care about empty squares.
			continue;
		}

		const bool white_color = position.get_color(square) == Color::White;
		const int pst_index = (white_color) ? i : 63 - i;
		const int pst_value = get_pst_value(piece_on_square, pst_index);
		// Checking if color is white and setting the sign to positive. If not white assumed it is black and sign is set to negative. If the color is empty then it does not matter what the sign is
		// since piece value will be zero.
		const int color_sign = (white_color) ? 1 : -1;
		score += color_sign * (get_piece_value(piece_on_square) + pst_value);
	}
	return score;
}
}  // namespace evaluation_sef