#include "movegen.hpp"

template<>
void generate_move<Piece::Rook>(const Position& position, Square from_square, MoveList& moves)
{
    Color player = position.get_player();

    Bitboard to_bitboard = generate_orthogonal_rays(position, from_square);

    for (uint8_t i = 0; i < 64; i++)
    {
        Square to_square(i);

        if (to_bitboard.read_by_square(to_square))
        {
            if (position.get_color(to_square) != player)
            {
                moves.emplace_back(from_square, to_square);
            }
        }
    }
}