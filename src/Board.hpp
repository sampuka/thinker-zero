#ifndef BOARD_HPP
#define BOARD_HPP

#include <array>
#include <cstdint>
#include <string>
#include <vector>

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
    {}

    Move(const Move &m)
        : fx(m.fx), fy(m.fy), tx(m.tx), ty(m.ty), promo(m.promo)
    {}

    std::string longform() const
    {
        std::string s;

        s += static_cast<char>('a'+fx);
        s += static_cast<char>('1'+fy);
        s += static_cast<char>('a'+tx);
        s += static_cast<char>('1'+ty);

        if (promo != Piece::None)
            s += static_cast<char>('0'+static_cast<std::uint8_t>(promo));

        return s;
    }

    std::uint8_t fx = 0;
    std::uint8_t fy = 0;
    std::uint8_t tx = 0;
    std::uint8_t ty = 0;
    Piece promo = Piece::None;
};

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

class Board
{
public:
    Board()
    {
        colors.at(static_cast<std::uint8_t>(Color::Empty)).board = ~0;

        // Setup standard board
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

        turn = Color::White;
        can_castle = {true, true};
    }

    Tile getTile(std::int8_t x_, std::int8_t y_) const
    {
        if (x_ < 0 || x_ > 7 || y_ < 0 || y_ > 7)
        {
            //std::cerr << "Get tile out of bounds (" << std::to_string(x_) << ", " << std::to_string(y_) << ")" << std::endl;
            return Tile{Color::Empty, Piece::None, true};
        }

        std::uint8_t x = static_cast<std::uint8_t>(x_);
        std::uint8_t y = static_cast<std::uint8_t>(y_);

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

    void setTile(std::int8_t x_, std::int8_t y_, Tile tile)
    {
        if (x_ < 0 || x_ > 7 || y_ < 0 || y_ > 7)
        {
            std::cerr << "Set tile out of bounds (" << std::to_string(x_) << ", " << std::to_string(y_) << ")" << std::endl;
        }

        std::uint8_t x = static_cast<std::uint8_t>(x_);
        std::uint8_t y = static_cast<std::uint8_t>(y_);

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

    std::vector<Move>& getMoves() const
    {
        find_movelist();

        return movelist;
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
    void find_movelist() const
    {
        if (movelist_found)
            return;

        Color enemy = Color::White;
        if (turn == Color::White)
            enemy = Color::Black;

        std::vector<Move> moves; // This will include illegal moves

        // For every tile ...
        for (std::uint8_t x = 0; x < 8; x++)
        {
            for (std::uint8_t y = 0; y < 8; y++)
            {
                const Tile tile = getTile(x, y);

                // Check if the piece is correct color
                if (turn != tile.color)
                {
                    continue;
                }

                // Quick method to expand pawn moves into all promotions
                const auto pawn_moves = [&](std::uint8_t fx, std::uint8_t fy, std::uint8_t tx, std::uint8_t ty)
                {
                    if (ty == 7 || ty == 0)
                    {
                        std::cout << "." << std::endl;
                        moves.emplace_back(fx, fy, tx, ty, Piece::Knight);
                        moves.emplace_back(fx, fy, tx, ty, Piece::Bishop);
                        moves.emplace_back(fx, fy, tx, ty, Piece::Rook);
                        moves.emplace_back(fx, fy, tx, ty, Piece::Queen);
                    }
                    else
                    {
                        moves.emplace_back(fx, fy, tx, ty);
                    }
                };

                // Quick method for iterating over knight moves
                const auto km = [](std::uint8_t i, std::uint8_t n)
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
                };

                switch (tile.piece)
                {
                    case Piece::Pawn:
                        {
                            if (turn == Color::White)
                            {
                                // If at starting pos
                                if (y == 1)
                                {
                                    pawn_moves(x, y, x, y+2);
                                }

                                // Standard pawn move
                                pawn_moves(x, y, x, y+1);

                                // Attacking moves
                                {
                                    const Tile t1 = getTile(x-1, y+1);
                                    if (t1.color == enemy && !t1.oob)
                                    {
                                        pawn_moves(x, y, x-1, y+1);
                                    }

                                    const Tile t2 = getTile(x+1, y+1);
                                    if (t1.color == enemy && !t2.oob)
                                    {
                                        pawn_moves(x, y, x+1, y+1);
                                    }
                                }

                                // En passant !!! Todo
                            }
                            else
                            {
                                // If at starting pos
                                if (y == 6)
                                {
                                    pawn_moves(x, y, x, y-2);
                                }

                                // Standard pawn move
                                pawn_moves(x, y, x, y-1);

                                // Attacking moves
                                {
                                    const Tile t1 = getTile(x-1, y-1);
                                    if (t1.color == enemy && !t1.oob)
                                    {
                                        pawn_moves(x, y, x-1, y-1);
                                    }

                                    const Tile t2 = getTile(x+1, y-1);
                                    if (t2.color == enemy && !t2.oob)
                                    {
                                        pawn_moves(x, y, x+1, y-1);
                                    }
                                }

                                // En passant !!! Todo
                            }
                        }
                        break;

                    case Piece::Knight:
                        {
                            for (std::uint8_t i = 0; i < 8; i++)
                            {
                                Tile t = getTile(x+km(i,0), y+km(i,1));
                                if (t.color != turn && !t.oob)
                                    moves.emplace_back(x, y, x+km(i,0), y+km(i,1));
                            }
                        }
                        break;

                    default:
                        {
                            std::cerr << "Unhandled piece type for finding move!" << std::endl;
                        }
                        break;
                }
            }
        }

        movelist = moves;
        movelist_found = true;
    }

    std::array<Bitboard, 3> colors;
    std::array<Bitboard, 6> pieces;

    Color turn = Color::White;
    std::array<bool, 2> can_castle = {true, true};

    mutable bool movelist_found = false;
    mutable std::vector<Move> movelist;
};

#endif
