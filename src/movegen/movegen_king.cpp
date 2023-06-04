#include "movegen.hpp"

#include "logging/logging.hpp"

template<>
void generate_moves<Piece::King>(const Position& position, MoveList& moves)
{
    Color player = position.get_player();
    
    Square from_square(0);

    bool found_king = false;

    // The least sane way to implement this
    for (uint8_t file = FILE_A; file <= FILE_H && !found_king; file++)
    {
        for (uint8_t rank = RANK_1; rank <= RANK_8 && !found_king; rank++)
        {
            from_square = Square(file, rank);

            if (position.get_color(from_square) == player && position.get_piece(from_square) == Piece::King)
            {
                found_king = true;
            }
        }
    }

    if (!found_king)
    {
        log_error("Didn't find king???");
    }

    uint8_t king_file = from_square.get_file();
    uint8_t king_rank = from_square.get_rank();

    uint8_t min_file = (king_file == FILE_A) ? FILE_A : (king_file - 1);
    uint8_t max_file = (king_file == FILE_H) ? FILE_H : (king_file + 1);

    uint8_t min_rank = (king_rank == RANK_1) ? RANK_1 : (king_rank - 1);
    uint8_t max_rank = (king_rank == RANK_8) ? RANK_8 : (king_rank + 1);
    
    for (uint8_t file = min_file; file <= max_file; file++)
    {
        for (uint8_t rank = min_rank; rank <= max_rank; rank++)
        {
            Square to_square(file, rank);

            if (from_square.get_data() != to_square.get_data() && position.get_color(to_square) != player)
            {
                moves.push_back(Move(from_square, to_square));
            }
        }
    }
}