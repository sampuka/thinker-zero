#ifndef MOVEGEN_MOVEGEN_HPP
#define MOVEGEN_MOVEGEN_HPP

#include "types/Position.hpp"

#include <vector>

using MoveList = std::vector<Move>; // Probably inefficient

template<Piece piece>
void generate_moves(const Position& position, MoveList& moves);

#endif // MOVEGEN_MOVEGEN_HPP
