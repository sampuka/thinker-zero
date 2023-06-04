#include "Move.hpp"

#include "logging/logging.hpp"
#include "util/bit_operations.hpp"

Move::Move(const std::string& move_string)
{
    if (move_string != "0000")
    {
        Square from_square(move_string.at(0) - 'a' + 1, move_string.at(1) - '0');
        Square to_square(move_string.at(2) - 'a' + 1, move_string.at(3) - '0');

        set_from_square(from_square);
        set_to_square(to_square);
    }
}

Move::Move(Square from, Square to)
{
    set_from_square(from);
    set_to_square(to);
}

void Move::set_to_square(Square to_square)
{
    set_bits<uint16_t, 0, 6>(encoded_move, to_square.get_data());
}

Square Move::get_to_square() const
{
    return Square(get_bits<uint16_t, 0, 6>(encoded_move));
}

void Move::set_from_square(Square from_square)
{
    set_bits<uint16_t, 6, 6>(encoded_move, from_square.get_data());
}

Square Move::get_from_square() const
{
    return Square(get_bits<uint16_t, 6, 6>(encoded_move));
}

void Move::set_type(MoveType type)
{
    set_bits<uint16_t, 12, 4>(encoded_move, static_cast<uint8_t>(type));
}

MoveType Move::get_type() const
{
    return static_cast<MoveType>(get_bits<uint16_t, 12, 4>(encoded_move));
}
