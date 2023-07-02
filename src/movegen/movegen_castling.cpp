#include "movegen.hpp"
#include "position/PositionAnalysis.hpp"

void generate_castling_move(const Position& position, MoveList& moves)
{
	Color player = position.get_player();

	Square from_square(FILE_E, RANK_1);

	if (player == Color::Black)
	{
		from_square = Square(FILE_E, RANK_8);
	}

	Position opponents_turn = position;
	opponents_turn.set_player(get_other_color(player));
	PositionAnalysis analysis(opponents_turn);

	Bitboard all_pieces = position.get_bitboard(Color::White) | position.get_bitboard(Color::Black);

	// Queenside castling
	if (position.may_queenside_castle())
	{
		// Check if castling tiles are clear
		Bitboard queenside_clear_bitboard = castle_ray_white_queen_clear;

		if (player == Color::Black)
		{
			queenside_clear_bitboard = castle_ray_black_queen_clear;
		}

		const bool clear = (queenside_clear_bitboard & all_pieces).empty();

		if (clear)
		{
			// Check if castling tiles are in check
			Bitboard queenside_safe_bitboard = castle_ray_white_queen_safe;

			if (player == Color::Black)
			{
				queenside_safe_bitboard = castle_ray_black_queen_safe;
			}

			const bool safe = (queenside_safe_bitboard & analysis.threatened_squares()).empty();

			if (safe)
			{
				moves.emplace_back(from_square, Square(FILE_C, from_square.get_rank()), MoveType::QueenCastle);
			}
		}
	}

	// Kingside
	if (position.may_kingside_castle())
	{
		// Check if castling tiles are clear
		Bitboard kingside_clear_bitboard = castle_ray_white_king_clear;

		if (player == Color::Black)
		{
			kingside_clear_bitboard = castle_ray_black_king_clear;
		}

		const bool clear = (kingside_clear_bitboard & all_pieces).empty();

		if (clear)
		{
			// Check if castling tiles are in check
			Bitboard kingside_safe_bitboard = castle_ray_white_king_safe;

			if (player == Color::Black)
			{
				kingside_safe_bitboard = castle_ray_black_king_safe;
			}

			const bool safe = (kingside_safe_bitboard & analysis.threatened_squares()).empty();

			if (safe)
			{
				moves.emplace_back(from_square, Square(FILE_G, from_square.get_rank()), MoveType::KingCastle);
			}
		}
	}
}