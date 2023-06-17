#ifndef EVALUATION_JUST_MATERIAL_HPP
#define EVALUATION_JUST_MATERIAL_HPP
#include "position/Position.hpp"

namespace evaluation_jm
{
int get_piece_value(const Piece& piece);

int evaluate_board(const Position& position);
}  // namespace evaluation_jm

#endif  // EVALUATION_JUST_MATERIAL_HPP
