#ifndef EVALUATION_EVALUATION_HPP
#define EVALUATION_EVALUATION_HPP
#include "position/Position.hpp"

namespace evaluation
{
int get_piece_value(const Piece& piece);

float evaluate_board(const Position& position);
}  // namespace evaluation

#endif  // EVALUATION_EVALUATION_HPP
