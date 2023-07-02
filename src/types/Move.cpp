#include "Move.hpp"

#include "types/conversions.hpp"

Move::Move(const std::string& move_string)
{
	if (move_string == "0000")
	{
		return;
	}

	Square from_square(move_string.at(0) - 'a' + 1, move_string.at(1) - '0');
	Square to_square(move_string.at(2) - 'a' + 1, move_string.at(3) - '0');

	set_from_square(from_square);
	set_to_square(to_square);

	if (move_string.size() == 5)
	{
		MoveType promo = convert_piece_to_promo(convert_char_to_piece(move_string.at(4)));
		set_type(promo);
	}
}