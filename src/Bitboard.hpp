#ifndef BITBOARD_HPP
#define BITBOARD_HPP

#include <cstdint>
#include <string>

class Bitboard
{
public:
    bool read(std::uint8_t x, std::uint8_t y) const
    {
        return board & (std::uint64_t{1} << (x + y*8));
    }

    constexpr void write(std::uint8_t x, std::uint8_t y, bool value)
    {
        //board ^= (-value ^ board) & (1 << (x + y*8));
        if (value)
            board |= (std::uint64_t{1} << (y*8+x));
        else
            board &= (~(std::uint64_t{1} << (y*8+x)));
    }

    std::uint8_t count() const
    {
        std::uint8_t c = 0;

        for (std::uint8_t i = 0; i < 64; i++)
        {
            if (board & (std::uint64_t{1} << i))
                c++;
        }

        return c;
    }

    std::uint8_t bitscan(std::uint8_t d) const
    {
        if ((d == 6) || (d == 7) || (d == 0) || (d == 1))
            return bitscan_forward();
        else
            return bitscan_backward();
    }

    std::uint8_t bitscan_forward() const
    {
        for (std::uint8_t i = 0; i < 64; i++)
        {
            if (board & (std::uint64_t{1} << i))
                return i;
        }

        return 0;
    }

    std::uint8_t bitscan_backward() const
    {
        for (std::uint8_t i = 63; i < 64; i--)
        {
            if (board & (std::uint64_t{1} << i))
                return i;
        }

        return 0;
    }

    void print() const
    {
        std::string s(8*9, ' ');

        for (std::uint8_t i = 0; i < 8; i++)
        {
            s.at(i*9+8) = '\n';
        }

        for (std::uint8_t x = 0; x < 8; x++)
        {
            for (std::uint8_t y = 0; y < 8; y++)
            {
                std::uint8_t pos = (7-y)*9+x;

                s.at(pos) = '0' + static_cast<std::uint8_t>(read(x,y));
                //std::cout << std::to_string(x) << ' ' << std::to_string(y) << " = " << std::to_string(static_cast<std::uint8_t>(read(x,y))) << std::endl;
            }
        }

        std::cout << s;
    }

    std::uint64_t board = 0;
};

#endif
