#ifndef TYPES_PIECE_HPP
#define TYPES_PIECE_HPP

#include <cstdint>

constexpr uint8_t types_of_pieces = 6;

enum class Piece : uint8_t
{
	Pawn = 0,
	Knight = 1,
	Bishop = 2,
	Rook = 3,
	Queen = 4,
	King = 5,
	Empty = 6
};
#endif  // TYPES_PIECE_HPP
