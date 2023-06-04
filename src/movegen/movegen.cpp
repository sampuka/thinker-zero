#include "movegen.hpp"

template<>
MoveList generate_moves<Piece::Pawn>(const Position& position)
{
    MoveList movelist;

    for (uint8_t file = FILE_A; file <= FILE_H; file++)
    {
        for (uint8_t rank = RANK_1; rank <= RANK_8; rank++)
        {
            Square from_square(file, rank);

            Color color = position.get_color(from_square);
            Piece piece = position.get_piece(from_square);

            if (color == Color::Black && piece == Piece::Pawn)
            {
                Square to_square(file, rank-1);
                movelist.push_back(Move(from_square, to_square));

                if (rank == RANK_7)
                {
                    Square double_to_square(file, rank-2);
                    movelist.push_back(Move(from_square, double_to_square));
                }
            }
        }
    }

    return movelist;
}

template<Piece piece>
MoveList generate_moves(const Position& position)
{
    (void)position;
    static_assert(false, "Movegen not implemented for this piece type");

    return {};
}