#include "movegen.hpp"

void add_pawn_move(Square from_square, Square to_square, MoveList& moves)
{
	uint8_t rank = to_square.get_rank();

	if (rank == RANK_1 || rank == RANK_8)
	{
		moves.emplace_back(from_square, to_square, MoveType::KnightPromo);
		moves.emplace_back(from_square, to_square, MoveType::BishopPromo);
		moves.emplace_back(from_square, to_square, MoveType::RookPromo);
		moves.emplace_back(from_square, to_square, MoveType::QueenPromo);
	}
	else
	{
		moves.emplace_back(from_square, to_square);
	}
}

template <>
void generate_move<Piece::Pawn>(const Position& position, Square from_square, MoveList& moves)
{
	Color player = position.get_player();
	uint8_t direction = (player == Color::White) ? 1 : -1;

	uint8_t file = from_square.get_file();
	uint8_t rank = from_square.get_rank();

	Square to_square_single(file, rank + direction);

	if (position.get_color(to_square_single) == Color::Empty)
	{
		add_pawn_move(from_square, to_square_single, moves);

		if ((player == Color::White && rank == RANK_2) || (player == Color::Black && rank == RANK_7))
		{
			Square to_square_double(file, rank + (direction * 2));

			if (position.get_color(to_square_double) == Color::Empty)
			{
				add_pawn_move(from_square, to_square_double, moves);
			}
		}
	}

	if (file != FILE_A)
	{
		Square to_square_capture_left(file - 1, rank + direction);

		if (position.get_color(to_square_capture_left) == get_other_color(player))
		{
			add_pawn_move(from_square, to_square_capture_left, moves);
		}
	}

	if (file != FILE_H)
	{
		Square to_square_capture_right(file + 1, rank + direction);

		if (position.get_color(to_square_capture_right) == get_other_color(player))
		{
			add_pawn_move(from_square, to_square_capture_right, moves);
		}
	}
}

template <Piece piece>
MoveList generate_moves(const Position& position, MoveList& moves)
{
	(void)position;
	static_assert(false, "Movegen not implemented for this piece type");

	return {};
}