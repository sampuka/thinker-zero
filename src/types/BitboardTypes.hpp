#ifndef TYPES_BITBOARDBYPIECE_HPP
#define TYPES_BITBOARDBYPIECE_HPP

#include "types/Bitboard.hpp"
#include "types/Color.hpp"
#include "types/Piece.hpp"

#include <array>

class BitboardByPiece
{
public:
    BitboardByPiece() = default;

    Piece find_on_square(Square square) const
    {
        for (uint8_t i = 0; i < bitboards.size(); i++)
        {
            if (bitboards[i].read_by_square(square))
            {
                return static_cast<Piece>(i);
            }
        }

        return Piece::Empty;
    }

    const Bitboard& operator[](enum Piece piece) const
    {
        return bitboards[static_cast<uint8_t>(piece)];
    }

    Bitboard& operator[](enum Piece piece)
    {
        return bitboards[static_cast<uint8_t>(piece)];
    }

private:
    std::array<Bitboard, types_of_pieces> bitboards;
};

class BitboardByColor
{
public:
    BitboardByColor() = default;

    Color find_on_square(Square square) const
    {
        for (uint8_t i = 0; i < bitboards.size(); i++)
        {
            if (bitboards[i].read_by_square(square))
            {
                return static_cast<Color>(i);
            }
        }

        return Color::Empty;
    }

    const Bitboard& operator[](enum Color color) const
    {
        return bitboards[static_cast<uint8_t>(color)];
    }

    Bitboard& operator[](enum Color color)
    {
        return bitboards[static_cast<uint8_t>(color)];
    }

private:
    std::array<Bitboard, 2> bitboards;
};

#endif // TYPES_BITBOARDBYPIECE_HPP
