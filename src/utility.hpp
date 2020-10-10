#ifndef UTILITY_HPP
#define UTILITY_HPP

#include <iostream>

template <typename T> int sgn(T val) {
    return (T(0) < val) - (val < T(0));
}

enum class Piece
{
    Pawn = 0,
    Knight,
    Bishop,
    Rook,
    Queen,
    King,
    None
};

enum class Color
{
    White = 0,
    Black,
    Empty
};

using Square = std::uint8_t;

// Assume king and from are aligned
bool is_aligned(Square king, Square from, Square to)
{
    std::int8_t x_sign = sgn((from%8)-(king%8));
    std::int8_t y_sign = sgn((from/8)-(king/8));

    std::int8_t x_diff = (to%8)-(king%8);
    std::int8_t y_diff = (to/8)-(king/8);

    if (x_sign == 0)
    {
        return x_diff == 0;
    }

    if (y_sign == 0)
    {
        return y_diff == 0;
    }

    return (x_diff / x_sign) == (y_diff / y_sign);
}

class Tile
{
public:
    Tile(Color color_, Piece piece_, bool oob_ = false)
        : color(color_), piece(piece_), oob(oob_)
    {}

    const Color color;
    const Piece piece;
    const bool oob; // Out of bounds
};

Tile char_to_tile(char c)
{
    switch (c)
    {
        case 'p': return Tile{Color::Black, Piece::Pawn};
        case 'P': return Tile{Color::White, Piece::Pawn};
        case 'n': return Tile{Color::Black, Piece::Knight};
        case 'N': return Tile{Color::White, Piece::Knight};
        case 'b': return Tile{Color::Black, Piece::Bishop};
        case 'B': return Tile{Color::White, Piece::Bishop};
        case 'r': return Tile{Color::Black, Piece::Rook};
        case 'R': return Tile{Color::White, Piece::Rook};
        case 'q': return Tile{Color::Black, Piece::Queen};
        case 'Q': return Tile{Color::White, Piece::Queen};
        case 'k': return Tile{Color::Black, Piece::King};
        case 'K': return Tile{Color::White, Piece::King};
        default: std::cout << "unhandled char in char_to_tile" << std::endl; return Tile{Color::Empty, Piece::None};
    }
}

char tile_to_char(Tile t)
{
    if (t.color == Color::Black)
    {
        switch (t.piece)
        {
            case Piece::Pawn:   return 'p';
            case Piece::Knight: return 'n';
            case Piece::Bishop: return 'b';
            case Piece::Rook:   return 'r';
            case Piece::Queen:  return 'q';
            case Piece::King:   return 'k';
            case Piece::None:   return '.';
            default:            return '?';
        }
    }
    else
    {
        switch (t.piece)
        {
            case Piece::Pawn:   return 'P';
            case Piece::Knight: return 'N';
            case Piece::Bishop: return 'B';
            case Piece::Rook:   return 'R';
            case Piece::Queen:  return 'Q';
            case Piece::King:   return 'K';
            case Piece::None:   return '.';
            default:            return '?';
        }
    }
}

// Quick method for iterating over knight moves
constexpr std::uint8_t km(std::uint8_t i, std::uint8_t n)
{
    if (n == 0)
    {
        switch (i)
        {
            case 0: return -1; break;
            case 1: return  1; break;
            case 2: return  2; break;
            case 3: return  2; break;
            case 4: return  1; break;
            case 5: return -1; break;
            case 6: return -2; break;
            case 7: return -2; break;
            default: return 0; break;
        }
    }
    else
    {
        switch (i)
        {
            case 0: return  2; break;
            case 1: return  2; break;
            case 2: return  1; break;
            case 3: return -1; break;
            case 4: return -2; break;
            case 5: return -2; break;
            case 6: return -1; break;
            case 7: return  1; break;
            default: return 0; break;
        }
    }
}

#endif
