#ifndef EVALUATION_EVALUATION_HPP
#define EVALUATION_EVALUATION_HPP
#include "position/Position.hpp"

int get_piece_value(const Piece& piece);

float evaluate_board(const Position& position);

#endif  // EVALUATION_EVALUATION_HPP
