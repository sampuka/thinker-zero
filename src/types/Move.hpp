#ifndef TYPES_MOVE_HPP
#define TYPES_MOVE_HPP

#include "types/Square.hpp"
#include "util/bit_operations.hpp"

#include <cstdint>
#include <string>

enum class MoveType : uint8_t
{
	Quiet = 0,
	Reserved1 = 1,
	Reserved2 = 2,
	Reserved3 = 3,
	Reserved4 = 4,
	Reserved5 = 5,
	Reserved6 = 6,
	Reserved7 = 7,
	KnightPromo = 8,
	BishopPromo = 9,
	RookPromo = 10,
	QueenPromo = 11,
	Reserved12 = 12,
	Reserved13 = 13,
	Reserved14 = 14,
	Reserved15 = 15
};

class Move
{
public:
	Move() = default;

	Move(const std::string& move_string)  // Construct the move on an empty board from a move string. "e7e8q", for instance
	{
		if (move_string != "0000")
		{
			Square from_square(move_string.at(0) - 'a' + 1, move_string.at(1) - '0');
			Square to_square(move_string.at(2) - 'a' + 1, move_string.at(3) - '0');

			set_from_square(from_square);
			set_to_square(to_square);
		}
	}

	Move(Square from, Square to, MoveType type = MoveType::Quiet)
	{
		set_from_square(from);
		set_to_square(to);
		set_type(type);
	}

	void set_to_square(Square to_square)
	{
		set_bits<uint16_t, 0, 6>(encoded_move, to_square.get_data());
	}

	Square get_to_square() const
	{
		return Square(get_bits<uint16_t, 0, 6>(encoded_move));
	}

	void set_from_square(Square from_square)
	{
		set_bits<uint16_t, 6, 6>(encoded_move, from_square.get_data());
	}

	Square get_from_square() const
	{
		return Square(get_bits<uint16_t, 6, 6>(encoded_move));
	}

	void set_type(MoveType type)
	{
		set_bits<uint16_t, 12, 4>(encoded_move, static_cast<uint8_t>(type));
	}

	MoveType get_type() const
	{
		return static_cast<MoveType>(get_bits<uint16_t, 12, 4>(encoded_move));
	}

private:
	uint16_t encoded_move = 0;
};

#endif // TYPES_MOVE_HPP
