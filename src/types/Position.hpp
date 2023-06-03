#ifndef TYPES_POSITION_HPP
#define TYPES_POSITION_HPP

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

private:

    BitboardByPiece bitboard_by_piece;
    BitboardByColor bitboard_by_color;
};

#endif // TYPES_POSITION_HPP