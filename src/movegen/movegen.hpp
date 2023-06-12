#ifndef MOVEGEN_MOVEGEN_HPP
#define MOVEGEN_MOVEGEN_HPP

#include "movegen_rays.hpp"
#include "position/Position.hpp"

#include <vector>

using MoveList = std::vector<Move>;  // Inefficient

MoveList generate_moves(const Position& position);

template <Piece piece>
void generate_move(const Position& position, Square from_square, MoveList& moves);

bool should_use_forward_scan(Ray ray);

Bitboard generate_from_ray(const Position& position, Square from_square, Ray ray);

Bitboard generate_orthogonal_rays(const Position& position, Square from_square);

Bitboard generate_diagonal_rays(const Position& position, Square from_square);

#endif  // MOVEGEN_MOVEGEN_HPP
