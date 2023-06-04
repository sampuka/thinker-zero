#ifndef MOVEGEN_MOVEGEN_HPP
#define MOVEGEN_MOVEGEN_HPP

#include "types/Position.hpp"

#include <vector>

using MoveList = std::vector<Move>; // Probably inefficient

template<Piece piece>
MoveList generate_moves(const Position& position);

#endif // MOVEGEN_MOVEGEN_HPP
