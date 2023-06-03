#ifndef TYPES_BITBOARD_HPP
#define TYPES_BITBOARD_HPP

#include "types/Square.hpp"

#include <cstdint>

class Bitboard
{
public:
    Bitboard() = default;

    bool read_by_square(Square square) const
    {
        if (board & (uint64_t{1} << square.get_data()))
        {
            return true;
        }
        else
        {
            return false;
        }
    }

    uint8_t read_bitcount() const
    {
        // Implement with fancy inline ASM thingy
        return 0;
    }

    void set_by_square(Square square)
    {
        board |= (uint64_t{1} << square.get_data());
    }

    void clear_by_square(Square square)
    {
        board &= ~(uint64_t{1} << square.get_data());
    }

private:
    uint64_t board = 0;
};

#endif // TYPES_BITBOARD_HPP
