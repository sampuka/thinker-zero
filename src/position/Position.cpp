#include "Position.hpp"

#include "logging/logging.hpp"
#include "types/conversions.hpp"

Position::Position()
{
	setup_standard_position();
}

void Position::reset()
{
	m_bitboard_by_piece = BitboardByPiece();
	m_bitboard_by_color = BitboardByColor();
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

	Color color = m_bitboard_by_color.find_on_square(from_square);
	Piece piece = m_bitboard_by_piece.find_on_square(from_square);

	if (color != get_player() || piece == Piece::Empty)
	{
		log_error("Illegal move from square (%d)", from_square.get_data());
		return;
	}

	// Remove piece from from square
	m_bitboard_by_color.clear_all_by_square(from_square);
	m_bitboard_by_piece.clear_all_by_square(from_square);

	// Remove piece about to be taken (if any)
	m_bitboard_by_color.clear_all_by_square(to_square);
	m_bitboard_by_piece.clear_all_by_square(to_square);

	// Place piece at to square
	Piece to_piece = convert_promo_to_piece(move.get_type());
	if (to_piece == Piece::Empty)
	{
		to_piece = piece;
	}
	m_bitboard_by_color[color].set_by_square(to_square);
	m_bitboard_by_piece[to_piece].set_by_square(to_square);

	MoveType type = move.get_type();

	// Perform rook move if castling
	if (type == MoveType::QueenCastle)
	{
		// Remove rook
		Square rook_from_square(FILE_A, to_square.get_rank());
		m_bitboard_by_color.clear_all_by_square(rook_from_square);
		m_bitboard_by_piece.clear_all_by_square(rook_from_square);

		// Place rook
		Square rook_to_square(FILE_D, to_square.get_rank());
		m_bitboard_by_color[color].set_by_square(rook_to_square);
		m_bitboard_by_piece[Piece::Rook].set_by_square(rook_to_square);
	}

	if (type == MoveType::KingCastle)
	{
		// Remove rook
		Square rook_from_square(FILE_H, to_square.get_rank());
		m_bitboard_by_color.clear_all_by_square(rook_from_square);
		m_bitboard_by_piece.clear_all_by_square(rook_from_square);

		// Place rook
		Square rook_to_square(FILE_F, to_square.get_rank());
		m_bitboard_by_color[color].set_by_square(rook_to_square);
		m_bitboard_by_piece[Piece::Rook].set_by_square(rook_to_square);
	}

	// Remove castling rights
	if (piece == Piece::King)
	{
		m_queenside_castling[static_cast<uint8_t>(color)] = false;
		m_kingside_castling[static_cast<uint8_t>(color)] = false;
	}

	constexpr Square queenside_white_rook(FILE_A, RANK_1);
	constexpr Square kingside_white_rook(FILE_H, RANK_1);
	constexpr Square queenside_black_rook(FILE_A, RANK_8);
	constexpr Square kingside_black_rook(FILE_H, RANK_8);

	if (to_square == queenside_white_rook || from_square == queenside_white_rook)
	{
		m_queenside_castling[static_cast<uint8_t>(Color::White)] = false;
	}
	if (to_square == kingside_white_rook || from_square == kingside_white_rook)
	{
		m_kingside_castling[static_cast<uint8_t>(Color::White)] = false;
	}
	if (to_square == queenside_black_rook || from_square == queenside_black_rook)
	{
		m_queenside_castling[static_cast<uint8_t>(Color::Black)] = false;
	}
	if (to_square == kingside_black_rook || from_square == kingside_black_rook)
	{
		m_kingside_castling[static_cast<uint8_t>(Color::Black)] = false;
	}

	m_player = get_other_color(m_player);
}

void Position::unmake_move(const Move& move)
{
	(void)move;
}

Piece Position::get_piece(Square square) const
{
	return m_bitboard_by_piece.find_on_square(square);
}

Color Position::get_color(Square square) const
{
	return m_bitboard_by_color.find_on_square(square);
}

void Position::set_square(Square square, Color color, Piece piece)
{
	m_bitboard_by_piece[piece].set_by_square(square);
	m_bitboard_by_color[color].set_by_square(square);
}

Color Position::get_player() const
{
	return m_player;
}

bool Position::may_queenside_castle() const
{
	return m_queenside_castling[static_cast<uint8_t>(get_player())];
}

bool Position::may_kingside_castle() const
{
	return m_kingside_castling[static_cast<uint8_t>(get_player())];
}

bool Position::may_white_queenside_castle() const
{
	return m_queenside_castling[static_cast<uint8_t>(Color::White)];
}

bool Position::may_white_kingside_castle() const
{
	return m_kingside_castling[static_cast<uint8_t>(Color::White)];
}

bool Position::may_black_queenside_castle() const
{
	return m_queenside_castling[static_cast<uint8_t>(Color::Black)];
}

bool Position::may_black_kingside_castle() const
{
	return m_kingside_castling[static_cast<uint8_t>(Color::Black)];
}

void Position::set_player(Color new_color)
{
	m_player = new_color;
}

Bitboard Position::get_bitboard(Color color) const
{
	return m_bitboard_by_color[color];
}

Bitboard Position::get_bitboard(Piece piece) const
{
	return m_bitboard_by_piece[piece];
}