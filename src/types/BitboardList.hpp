#ifndef TYPES_BITBOARDBYPIECE_HPP
#define TYPES_BITBOARDBYPIECE_HPP

#include "types/Bitboard.hpp"
#include "types/Color.hpp"
#include "types/Piece.hpp"

#include <array>

template <typename T, uint8_t size>
class BitboardList
{
public:
	BitboardList() = default;

	T find_on_square(Square square) const
	{
		for (uint8_t i = 0; i < bitboards.size(); i++)
		{
			if (bitboards[i].read_by_square(square))
			{
				return static_cast<T>(i);
			}
		}

		return T::Empty;
	}

	void clear_all_by_square(Square square)
	{
		for (uint8_t i = 0; i < bitboards.size(); i++)
		{
			bitboards[i].clear_by_square(square);
		}
	}

	const Bitboard& operator[](T key) const
	{
		return bitboards[static_cast<uint8_t>(key)];
	}

	Bitboard& operator[](T key)
	{
		return bitboards[static_cast<uint8_t>(key)];
	}

private:
	std::array<Bitboard, size> bitboards;
};

using BitboardByPiece = BitboardList<Piece, types_of_pieces>;
using BitboardByColor = BitboardList<Color, 2>;

#endif  // TYPES_BITBOARDBYPIECE_HPP
