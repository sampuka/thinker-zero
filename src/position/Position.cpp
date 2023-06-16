#include "Position.hpp"

#include "logging/logging.hpp"
#include "types/conversions.hpp"

Position::Position()
{
}

void Position::reset()
{
	bitboard_by_piece = BitboardByPiece();
	bitboard_by_color = BitboardByColor();
}

void Position::setup_standard_position()
{
	reset();

	// Pawn rows
	for (uint8_t file = FILE_A; file <= FILE_H; file++)
	{
		set_square(Square(file, RANK_2), Color::White, Piece::Pawn);
		set_square(Square(file, RANK_7), Color::Black, Piece::Pawn);
	}

	// Knights
	set_square(Square(FILE_B, RANK_1), Color::White, Piece::Knight);
	set_square(Square(FILE_G, RANK_1), Color::White, Piece::Knight);
	set_square(Square(FILE_B, RANK_8), Color::Black, Piece::Knight);
	set_square(Square(FILE_G, RANK_8), Color::Black, Piece::Knight);

	// Bishops
	set_square(Square(FILE_C, RANK_1), Color::White, Piece::Bishop);
	set_square(Square(FILE_F, RANK_1), Color::White, Piece::Bishop);
	set_square(Square(FILE_C, RANK_8), Color::Black, Piece::Bishop);
	set_square(Square(FILE_F, RANK_8), Color::Black, Piece::Bishop);

	// Rooks
	set_square(Square(FILE_A, RANK_1), Color::White, Piece::Rook);
	set_square(Square(FILE_H, RANK_1), Color::White, Piece::Rook);
	set_square(Square(FILE_A, RANK_8), Color::Black, Piece::Rook);
	set_square(Square(FILE_H, RANK_8), Color::Black, Piece::Rook);

	// Queens
	set_square(Square(FILE_D, RANK_1), Color::White, Piece::Queen);
	set_square(Square(FILE_D, RANK_8), Color::Black, Piece::Queen);

	// Kings
	set_square(Square(FILE_E, RANK_1), Color::White, Piece::King);
	set_square(Square(FILE_E, RANK_8), Color::Black, Piece::King);
}

void Position::make_move(const Move& move)
{
	Square from_square = move.get_from_square();
	Square to_square = move.get_to_square();

	Color color = bitboard_by_color.find_on_square(from_square);
	Piece piece = bitboard_by_piece.find_on_square(from_square);

	if (color != get_player() || piece == Piece::Empty)
	{
		log_error("Illegal move from square (%d)", from_square.get_data());
		return;
	}

	// Remove piece from from square
	bitboard_by_color.clear_all_by_square(from_square);
	bitboard_by_piece.clear_all_by_square(from_square);

	// Remove piece about to be taken (if any)
	bitboard_by_color.clear_all_by_square(to_square);
	bitboard_by_piece.clear_all_by_square(to_square);

	// Place piece at to square
	Piece to_piece = convert_promo_to_piece(move.get_type());
	if (to_piece == Piece::Empty)
	{
		to_piece = piece;
	}
	bitboard_by_color[color].set_by_square(to_square);
	bitboard_by_piece[to_piece].set_by_square(to_square);

	player = get_other_color(player);
}

void Position::unmake_move(const Move& move)
{
	(void)move;
}

Piece Position::get_piece(Square square) const
{
	return bitboard_by_piece.find_on_square(square);
}

Color Position::get_color(Square square) const
{
	return bitboard_by_color.find_on_square(square);
}

void Position::set_square(Square square, Color color, Piece piece)
{
	bitboard_by_piece[piece].set_by_square(square);
	bitboard_by_color[color].set_by_square(square);
}

Color Position::get_player() const
{
	return player;
}

void Position::set_player(Color new_color)
{
	player = new_color;
}

Bitboard Position::get_bitboard(Color color) const
{
	return bitboard_by_color[color];
}

Bitboard Position::get_bitboard(Piece piece) const
{
	return bitboard_by_piece[piece];
}