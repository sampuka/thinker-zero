#ifndef BITBOARD_HPP
#define BITBOARD_HPP

#include <cstdint>
#include <string>

using Bitboard = std::uint64_t;

constexpr bool bitboard_read(const Bitboard &board, std::uint8_t x, std::uint8_t y)
{
    return board & (std::uint64_t{1} << (x + y*8));
}

constexpr bool bitboard_read(const Bitboard &board, std::uint8_t i)
{
    return board & (std::uint64_t{1} << i);
}

constexpr void bitboard_set(Bitboard &board, std::uint8_t x, std::uint8_t y)
{
    board |= (std::uint64_t{1} << (y*8+x));
}

constexpr void bitboard_set(Bitboard &board, std::uint8_t i)
{
    board |= (std::uint64_t{1} << i);
}

constexpr void bitboard_unset(Bitboard &board, std::uint8_t x, std::uint8_t y)
{
    board &= (~(std::uint64_t{1} << (y*8+x)));
}

constexpr void bitboard_unset(Bitboard &board, std::uint8_t i)
{
    board &= (~(std::uint64_t{1} << i));
}

std::uint8_t bitboard_count(const Bitboard &board)
{
    std::uint64_t c = 0;

    asm(
            "popcnt %1,%0\n"
            : "=r"(c)
            : "b"(board)
       );

    return c;
}

std::uint8_t bitboard_bitscan_forward(const Bitboard &board)
{
    std::uint64_t res=0;

    asm(
            "bsf %1,%0\n"
            : "=r"(res)
            : "b"(board)
       );

    return res;
}

std::uint8_t bitboard_bitscan_forward_pop(Bitboard &board)
{
    std::uint64_t res=0;

    asm(
            "bsf %1,%0\n"
            : "=r"(res)
            : "b"(board)
       );

    bitboard_unset(board, res);

    return res;
}

std::uint8_t bitboard_bitscan_backward(const Bitboard &board)
{
    std::uint64_t res=0;

    asm(
            "bsr %1,%0\n"
            : "=r"(res)
            : "b"(board)
       );

    return res;
}

std::uint8_t bitboard_bitscan_backward_pop(Bitboard &board)
{
    std::uint64_t res=0;

    asm(
            "bsr %1,%0\n"
            : "=r"(res)
            : "b"(board)
       );

    bitboard_unset(board, res);

    return res;
}

std::uint8_t bitboard_bitscan(const Bitboard &board, std::uint8_t d)
{
    if ((d == 6) || (d == 7) || (d == 0) || (d == 1))
        return bitboard_bitscan_forward(board);
    else
        return bitboard_bitscan_backward(board);
}

void bitboard_print(const Bitboard &board)
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

            s.at(pos) = '0' + static_cast<std::uint8_t>(bitboard_read(board, x,y));
        }
    }

    std::cout << s;
}

#endif
