#ifndef BOARD_HPP
#define BOARD_HPP

#include <cstdint>
#include <string>
#include <array>

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

class Bitboard
{
public:
    bool read(std::uint8_t x, std::uint8_t y) const
    {
        return board & (std::uint64_t{1} << (x + y*8));
    }

    void write(std::uint8_t x, std::uint8_t y, bool value)
    {
        //board ^= (-value ^ board) & (1 << (x + y*8));
        if (value)
            board |= (std::uint64_t{1} << (y*8+x));
        else
            board &= (~(std::uint64_t{1} << (y*8+x)));
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

class Move
{
public:
    Move() = delete;

    // From x, from y, to x, to y
    Move(std::uint8_t fx_, std::uint8_t fy_, std::uint8_t tx_, std::uint8_t ty_, Piece promo_ = Piece::None)
        : fx(fx_), fy(fy_), tx(tx_), ty(ty_), promo(promo_)
    {
    };

    std::string longform() const
    {
        if (promo == Piece::None)
            return std::to_string('a'+fx) + std::to_string('0'+fy) + std::to_string('a'+tx) + std::to_string('0'+ty);
        else
            return std::to_string('a'+fx) + std::to_string('0'+fy) + std::to_string('a'+tx) + std::to_string('0'+ty) + std::to_string('0'+static_cast<std::uint8_t>(promo));
    };

    const std::uint8_t fx;
    const std::uint8_t fy;
    const std::uint8_t tx;
    const std::uint8_t ty;
    const Piece promo;
};

class Tile
{
public:
    Tile(Color color_, Piece piece_)
        : color(color_), piece(piece_)
    {}

    const Color color;
    const Piece piece;
};

class Board
{
public:
    Board()
    {
        colors.at(static_cast<std::uint8_t>(Color::Empty)).board = ~0;

        for (std::uint8_t i = 0; i < 8; i++)
        {
            setTile(i, 1, {Color::White, Piece::Pawn});
            setTile(i, 6, {Color::Black, Piece::Pawn});
        }

        // White backrow
        setTile(0, 0, {Color::White, Piece::Rook});
        setTile(1, 0, {Color::White, Piece::Knight});
        setTile(2, 0, {Color::White, Piece::Bishop});
        setTile(3, 0, {Color::White, Piece::Queen});
        setTile(4, 0, {Color::White, Piece::King});
        setTile(5, 0, {Color::White, Piece::Bishop});
        setTile(6, 0, {Color::White, Piece::Knight});
        setTile(7, 0, {Color::White, Piece::Rook});

        // Black backrow
        setTile(0, 7, {Color::Black, Piece::Rook});
        setTile(1, 7, {Color::Black, Piece::Knight});
        setTile(2, 7, {Color::Black, Piece::Bishop});
        setTile(3, 7, {Color::Black, Piece::Queen});
        setTile(4, 7, {Color::Black, Piece::King});
        setTile(5, 7, {Color::Black, Piece::Bishop});
        setTile(6, 7, {Color::Black, Piece::Knight});
        setTile(7, 7, {Color::Black, Piece::Rook});
    };

    Tile getTile(std::uint8_t x, std::uint8_t y) const
    {
        Color color;
        Piece piece;

        for (std::uint8_t c = 0; c < 3; c++)
        {
            if (colors.at(c).read(x, y))
            {
                color = static_cast<Color>(c);
            }
        }

        for (std::uint8_t p = 0; p < 6; p++)
        {
            if (pieces.at(p).read(x, y))
            {
                piece = static_cast<Piece>(p);
            }
        }

        return Tile{color, piece};
    }

    void setTile(std::uint8_t x, std::uint8_t y, Tile tile)
    {
        for (std::uint8_t c = 0; c < 3; c++)
        {
            colors.at(c).write(x, y, false);
        }

        for (std::uint8_t p = 0; p < 6; p++)
        {
            pieces.at(p).write(x, y, false);
        }

        colors.at(static_cast<std::uint8_t>(tile.color)).write(x, y, true);
        pieces.at(static_cast<std::uint8_t>(tile.piece)).write(x, y, true);
    }

    void print() const
    {
        std::string s(10*11, ' ');

        for (std::uint8_t i = 0; i < 10; i++)
            s.at(i*11+10) = '\n';

        for (std::uint8_t x = 0; x < 8; x++)
        {
            for (std::uint8_t y = 0; y < 8; y++)
            {
                std::uint8_t pos = ((8-y)*11)+x+1;

                Tile tile = getTile(x, y);

                if (tile.color == Color::White)
                {
                    s.at(pos) = '0'+static_cast<std::uint8_t>(tile.piece);
                }
                else if (tile.color == Color::Black)
                {
                    s.at(pos) = 'a'+static_cast<std::uint8_t>(tile.piece);
                }
                else
                {
                    s.at(pos) = '.';
                }
            }
        }

        std::cout << s;

        std::cout << "Colors (white/black/empty)\n";

        //colors.at(0).print();
        for (std::uint8_t c = 0; c < 3; c++)
        {
            colors.at(c).print();
            std::cout << '\n';
        }

        std::cout << "Pieces (p,k,b,r,q,k)\n";

        for (std::uint8_t p = 0; p < 6; p++)
        {
            pieces.at(p).print();
            std::cout << '\n';
        }

        std::cout << std::flush;
    }

private:
    std::array<Bitboard, 3> colors;
    std::array<Bitboard, 6> pieces;

    Color turn = Color::White;
    std::array<bool, 2> castleable = {true, true};
};

#endif
