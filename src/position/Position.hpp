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
    void unmake_move(const Move& move); // Is this implementation possible in this context? Is it needed?

    Piece get_piece(Square square) const;
    Color get_color(Square square) const;
    
    void set_square(Square square, Color color, Piece piece);

    Color get_player() const;
    void set_player(Color new_color);

    Bitboard get_bitboard(Color color) const;
    Bitboard get_bitboard(Piece piece) const;

private:
    Color player = Color::White;
    BitboardByPiece bitboard_by_piece;
    BitboardByColor bitboard_by_color;
};

#endif // POSITION_POSITION_HPP
