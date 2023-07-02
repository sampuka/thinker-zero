#include "movegen.hpp"

#include "logging/logging.hpp"
#include "movegen/movegen.hpp"
#include "position/PositionAnalysis.hpp"

MoveList generate_legal_moves(const Position& position)
{
	MoveList pseudolegal_moves = generate_pseudolegal_moves(position);

	MoveList legal_moves;
	for (const Move& move : pseudolegal_moves)
	{
		Position new_pos = position;
		new_pos.make_move(move);

		PositionAnalysis analysis(new_pos);

		if (!analysis.king_in_check())
		{
			legal_moves.push_back(move);
		}
	}

	generate_castling_move(position, legal_moves);

	return legal_moves;
}

MoveList generate_pseudolegal_moves(const Position& position)
{
	MoveList moves;

	Color player = position.get_player();

	// The least sane way to implement this
	for (uint8_t file = FILE_A; file <= FILE_H; file++)
	{
		for (uint8_t rank = RANK_1; rank <= RANK_8; rank++)
		{
			Square from_square(file, rank);

			if (position.get_color(from_square) == player)
			{
				Piece piece = position.get_piece(from_square);

				switch (piece)
				{
					case Piece::Pawn:
					{
						generate_move<Piece::Pawn>(position, from_square, moves);
						break;
					}
					case Piece::Knight:
					{
						generate_move<Piece::Knight>(position, from_square, moves);
						break;
					}
					case Piece::Bishop:
					{
						generate_move<Piece::Bishop>(position, from_square, moves);
						break;
					}
					case Piece::Rook:
					{
						generate_move<Piece::Rook>(position, from_square, moves);
						break;
					}
					case Piece::Queen:
					{
						generate_move<Piece::Queen>(position, from_square, moves);
						break;
					}
					case Piece::King:
					{
						generate_move<Piece::King>(position, from_square, moves);
						break;
					}
					default:
					{
						log_error("Color without piece??");
						break;
					}
				}
			}
		}
	}

	return moves;
}

bool should_use_forward_scan(Ray ray)
{
	return (ray == Ray::E || ray == Ray::NE || ray == Ray::N || ray == Ray::NW);
}

Bitboard generate_from_ray(const Position& position, Square from_square, Ray ray)
{
	Color player = position.get_player();

	Bitboard full_ray = movegen_rays[static_cast<uint8_t>(ray)][from_square.get_data()];

	Bitboard player_pieces = position.get_bitboard(player);

	Bitboard all_pieces = player_pieces | position.get_bitboard(get_other_color(player));

	Bitboard collisions = full_ray & all_pieces;

	Square first_collision(collisions.scan_backward());

	if (should_use_forward_scan(ray))
	{
		first_collision = Square(collisions.scan_forward());
	}

	Bitboard shade_ray = movegen_rays[static_cast<uint8_t>(ray)][first_collision.get_data()];

	Bitboard visible = full_ray & ~shade_ray;

	Bitboard reachable = visible & ~player_pieces;

	return reachable;
}

Bitboard generate_orthogonal_rays(const Position& position, Square from_square)
{
	Bitboard east = generate_from_ray(position, from_square, Ray::E);
	Bitboard south = generate_from_ray(position, from_square, Ray::S);
	Bitboard west = generate_from_ray(position, from_square, Ray::W);
	Bitboard north = generate_from_ray(position, from_square, Ray::N);

	return east | south | west | north;
}

Bitboard generate_diagonal_rays(const Position& position, Square from_square)
{
	Bitboard northeast = generate_from_ray(position, from_square, Ray::NE);
	Bitboard northwest = generate_from_ray(position, from_square, Ray::NW);
	Bitboard southeast = generate_from_ray(position, from_square, Ray::SE);
	Bitboard southwest = generate_from_ray(position, from_square, Ray::SW);

	return northeast | northwest | southeast | southwest;
}