#ifndef ZOBRIST_HPP
#define ZOBRIST_HPP

#include <array>
#include <limits>
#include <random>

//std::random_device zobrist_rd;
std::mt19937 zobrist_eng(1337);
std::uniform_int_distribution<std::uint64_t> zobrist_dist(0, std::numeric_limits<std::uint64_t>::max());

// Zobrist values for pieces
// INDEX IN ORDER OF COLOR (2), PIECE TYPE (6), SQUARE (64)
const std::array<std::array<std::array<std::uint64_t, 64>, 6>, 2> zobrist_pieces = [&]()
{
    std::array<std::array<std::array<std::uint64_t, 64>, 6>, 2> z;

    for (std::uint8_t sq = 0; sq < 64; sq++)
    {
        for (std::uint8_t p = 0; p < 6; p++)
        {
            for (std::uint8_t c = 0; c < 2; c++)
            {
                z[c][p][sq] = zobrist_dist(zobrist_eng);
            }
        }
    }

    return z;
}();

// Zobrist value for black to move
const std::uint64_t zobrist_black = zobrist_dist(zobrist_eng);

// Four zobrist values for castling rights
// INDEX IN ORDER OF WHITE/BLACK (2), KINGSIDE/QUEENSIDE (2)
const std::array<std::array<std::uint64_t, 2>, 2> zobrist_castles = [&]()
{
    std::array<std::array<std::uint64_t, 2>, 2> z;

    for (std::uint8_t s = 0; s < 2; s++)
    {
        for (std::uint8_t c = 0; c < 2; c++)
        {
            z[c][s] = zobrist_dist(zobrist_eng);
        }
    }

    return z;
}();

// Zobrist values for en passant columns
const std::array<std::uint64_t, 8> zobrist_ep = [&]()
{
    std::array<std::uint64_t, 8> z;

    for (std::uint8_t c = 0; c < 8; c++)
    {
        z[c] = zobrist_dist(zobrist_eng);
    }

    return z;
}();

#endif
