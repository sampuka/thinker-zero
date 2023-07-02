#ifndef POSITION_POSITION_HPP
#define POSITION_POSITION_HPP

#include "types/BitboardList.hpp"
#include "types/Move.hpp"

#include <array>

class Position
{
public:
	Position();

	void reset();

	void setup_standard_position();

	void make_move(const Move& move);
	void unmake_move(const Move& move);  // Is this implementation possible in this context? Is it needed?

	Piece get_piece(Square square) const;
	Color get_color(Square square) const;

	void set_square(Square square, Color color, Piece piece);

	Bitboard get_bitboard(Color color) const;
	Bitboard get_bitboard(Piece piece) const;

	Color get_player() const;
	void set_player(Color new_color);

	bool may_queenside_castle() const;
	bool may_kingside_castle() const;

	bool may_white_queenside_castle() const;
	bool may_white_kingside_castle() const;
	bool may_black_queenside_castle() const;
	bool may_black_kingside_castle() const;

private:
	BitboardByPiece m_bitboard_by_piece;
	BitboardByColor m_bitboard_by_color;

	// Encoded efficiently later
	Color m_player = Color::White;
	std::array<bool, 2> m_queenside_castling = {true, true};  // Indexed by Color
	std::array<bool, 2> m_kingside_castling = {true, true};
};

#endif  // POSITION_POSITION_HPP
