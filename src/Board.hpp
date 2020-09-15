#ifndef BOARD_HPP
#define BOARD_HPP

#include <array>
#include <cstdint>
#include <iostream>
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

    Move(std::string s)
    {
        fx = static_cast<std::uint8_t>(s.at(0)-'a');
        fy = static_cast<std::uint8_t>(s.at(1)-'1');
        tx = static_cast<std::uint8_t>(s.at(2)-'a');
        ty = static_cast<std::uint8_t>(s.at(3)-'1');

        if (s.size() == 5)
        {
            switch (s.at(4))
            {
                case 'k': promo = Piece::Knight; break;
                case 'b': promo = Piece::Bishop; break;
                case 'r': promo = Piece::Rook;   break;
                case 'q': promo = Piece::Queen;  break;
                default: break;
            }
        }
    }

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

        switch (promo)
        {
            case Piece::None: break;
            case Piece::Knight: s += 'k'; break;
            case Piece::Bishop: s += 'b'; break;
            case Piece::Rook:   s += 'r'; break;
            case Piece::Queen:  s += 'q'; break;
            default: s += '?'; break;
        }

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
        can_castle.at(0).at(0) = true;
        can_castle.at(0).at(1) = true;
        can_castle.at(1).at(0) = true;
        can_castle.at(1).at(1) = true;

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

        rook_starts = {0, 7};
        king_start = 4;
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

        Color color = Color::Empty;
        Piece piece = Piece::None;

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

        if (tile.piece != Piece::None)
        {
            pieces.at(static_cast<std::uint8_t>(tile.piece)).write(x, y, true);
        }
    }

    std::vector<Move> getMoves() const
    {
        find_movelist();

        return movelist;
    }

    bool canCaptureKing() const
    {
        find_movelist(false);

        Color enemy = Color::White;
        if (turn == Color::White)
            enemy = Color::Black;

        for (const Move &move : movelist)
        {
            Tile t = getTile(move.tx, move.ty);

            if (t.color == enemy && t.piece == Piece::King)
            {
                return true;
            }
        }

        return false;
    }

    void performMove(Move move)
    {
        Tile from = getTile(move.fx, move.fy);
        //Tile to = getTile(move.tx, move.ty);

        if (move.promo == Piece::None)
            setTile(move.tx, move.ty, from);
        else
            setTile(move.tx, move.ty, Tile{from.color, move.promo});

        setTile(move.fx, move.fy, Tile{Color::Empty, Piece::None});

        // Castling move
        if (from.piece == Piece::King && std::abs(move.tx - move.fx) >= 2)
        {
            // Kingside
            if (move.tx > move.fx)
            {
                // Find kingside rook
                std::uint8_t x = 7;

                while (getTile(x, move.fy).piece != Piece::Rook)
                    x--;

                setTile(5, move.fy, Tile{from.color, Piece::Rook});
                setTile(x, move.fy, Tile{Color::Empty, Piece::None});
            }

            // Queenside
            if (move.tx < move.fx)
            {
                // Find queenside rook
                std::uint8_t x = 0;

                while (getTile(x, move.fy).piece != Piece::Rook)
                    x++;

                setTile(3, move.fy, Tile{from.color, Piece::Rook});
                setTile(x, move.fy, Tile{Color::Empty, Piece::None});
            }
        }

        // Handle castling priviledges if king move
        if (from.piece == Piece::King)
        {
            can_castle.at(static_cast<std::uint8_t>(turn)).at(0) = false;
            can_castle.at(static_cast<std::uint8_t>(turn)).at(1) = false;
        }

        // Handle castling priviledges if rook move
        if (from.piece == Piece::Rook)
        {
            if (move.fx == rook_starts.at(0))
            {
                can_castle.at(static_cast<std::uint8_t>(turn)).at(0) = false;
            }

            if (move.fx == rook_starts.at(1))
            {
                can_castle.at(static_cast<std::uint8_t>(turn)).at(1) = false;
            }
        }

        if (turn == Color::White)
            turn = Color::Black;
        else
            turn = Color::White;

        movelist_found = false;
        movelist.clear();
    }

    void print(std::ostream &os = std::cout) const
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

        if (turn == Color::White)
            os << "White to move\n";
        else
            os << "Black to move\n";

        os << s;

        /*
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
        */

        os << "Can castle: " << std::to_string(can_castle.at(0).at(0)) << std::to_string(can_castle.at(0).at(1)) << std::to_string(can_castle.at(1).at(0)) << std::to_string(can_castle.at(1).at(1)) << '\n';

        os << std::flush;
    }

private:
    void find_movelist(bool clean_list = true) const
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

                // Quick method for iterating over bishop moves
                const auto bm = [](std::uint8_t d, std::uint8_t n)
                {
                    switch (d)
                    {
                        case 0: return n ?  1 :  1; break;
                        case 1: return n ?  1 : -1; break;
                        case 2: return n ? -1 :  1; break;
                        case 3: return n ? -1 : -1; break;
                        default: return 1; break;
                    }
                };

                // Quick method for iterating over rook moves
                const auto rm = [](std::uint8_t d, std::uint8_t n)
                {
                    switch (d)
                    {
                        case 0: return n ?  1 :  0; break;
                        case 1: return n ?  0 :  1; break;
                        case 2: return n ? -1 :  0; break;
                        case 3: return n ?  0 : -1; break;
                        default: return 1; break;
                    }
                };

                switch (tile.piece)
                {
                    case Piece::Pawn:
                        {
                            if (turn == Color::White)
                            {
                                // Standard pawn move
                                if (getTile(x, y+1).color == Color::Empty)
                                {
                                    pawn_moves(x, y, x, y+1);

                                    // If at starting pos
                                    if (y == 1 && getTile(x, y+2).color == Color::Empty)
                                    {
                                        pawn_moves(x, y, x, y+2);
                                    }
                                }

                                // Attacking moves
                                {
                                    const Tile t1 = getTile(x-1, y+1);
                                    if (t1.color == enemy && !t1.oob)
                                    {
                                        pawn_moves(x, y, x-1, y+1);
                                    }

                                    const Tile t2 = getTile(x+1, y+1);
                                    if (t2.color == enemy && !t2.oob)
                                    {
                                        pawn_moves(x, y, x+1, y+1);
                                    }
                                }

                                // En passant !!! Todo
                            }
                            else
                            {
                                // Standard pawn move
                                if (getTile(x, y-1).color == Color::Empty)
                                {
                                    pawn_moves(x, y, x, y-1);

                                    // If at starting pos
                                    if (y == 6 && getTile(x, y-2).color == Color::Empty)
                                    {
                                        pawn_moves(x, y, x, y-2);
                                    }
                                }

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

                    case Piece::Bishop:
                        {
                            for (std::uint8_t d = 0; d < 4; d++)
                            {
                                for (std::uint8_t i = 1; ; i++)
                                {
                                    std::int8_t x_ = x+i*bm(d,0);
                                    std::int8_t y_ = y+i*bm(d,1);

                                    Tile t = getTile(x_, y_);

                                    if (t.oob || t.color == turn)
                                        break;

                                    moves.emplace_back(x, y, x_, y_);

                                    if (t.color == enemy)
                                        break;
                                }
                            }
                        }
                        break;

                    case Piece::Rook:
                        {
                            for (std::uint8_t d = 0; d < 4; d++)
                            {
                                for (std::uint8_t i = 1; ; i++)
                                {
                                    std::int8_t x_ = x+i*rm(d,0);
                                    std::int8_t y_ = y+i*rm(d,1);

                                    Tile t = getTile(x_, y_);

                                    if (t.oob || t.color == turn)
                                        break;

                                    moves.emplace_back(x, y, x_, y_);

                                    if (t.color == enemy)
                                        break;
                                }
                            }
                        }
                        break;

                    case Piece::Queen:
                        {
                            // Bishop moves
                            for (std::uint8_t d = 0; d < 4; d++)
                            {
                                for (std::uint8_t i = 1; ; i++)
                                {
                                    std::int8_t x_ = x+i*bm(d,0);
                                    std::int8_t y_ = y+i*bm(d,1);

                                    Tile t = getTile(x_, y_);

                                    if (t.oob || t.color == turn)
                                        break;

                                    moves.emplace_back(x, y, x_, y_);

                                    if (t.color == enemy)
                                        break;
                                }
                            }

                            // Rook moves
                            for (std::uint8_t d = 0; d < 4; d++)
                            {
                                for (std::uint8_t i = 1; ; i++)
                                {
                                    std::int8_t x_ = x+i*rm(d,0);
                                    std::int8_t y_ = y+i*rm(d,1);

                                    Tile t = getTile(x_, y_);

                                    if (t.oob || t.color == turn)
                                        break;

                                    moves.emplace_back(x, y, x_, y_);

                                    if (t.color == enemy)
                                        break;
                                }
                            }
                        }
                        break;

                    case Piece::King:
                        {
                            for (std::int8_t x_ = x-1; x_ <= x+1; x_++)
                            {
                                for (std::int8_t y_ = y-1; y_ <= y+1; y_++)
                                {
                                    if (x_ == x && y_ == y)
                                        continue;

                                    Tile t = getTile(x_, y_);

                                    if (t.oob || t.color == turn)
                                        continue;

                                    moves.emplace_back(x, y, x_, y_);
                                }
                            }

                            // Queen side castling
                            if (can_castle.at(static_cast<std::uint8_t>(turn)).at(0))
                            {
                                bool clear = true;

                                for (std::uint8_t x_ = rook_starts.at(0)+1; x_ < king_start; x_++)
                                {
                                    if (getTile(x_, y).color != Color::Empty)
                                    {
                                        clear = false;
                                        break;
                                    }
                                }

                                if (clear)
                                {
                                    Board next(*this);
                                    next.performMove(Move{x, y, 2, y}); // I think this line is problematic for chess960

                                    std::vector<Move> next_moves = next.getMoves();

                                    for (const Move &move : next_moves)
                                    {
                                        if (move.ty == y && (move.tx >= rook_starts.at(0) && move.tx <= king_start))
                                        {
                                            clear = false;
                                            break;
                                        }
                                    }

                                    if (clear)
                                    {
                                        moves.emplace_back(x, y, 2, y);
                                        //std::cout << "CAN QUEENSIDE CASTLE :)" << std::endl;
                                    }
                                }
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

        if (clean_list)
        {
            for (const Move &move : moves)
            {
                Board next(*this);

                next.performMove(move);

                if (!next.canCaptureKing())
                {
                    movelist.push_back(move);
                }
            }
        }
        else
        {
            movelist = moves;
        }

        //movelist = moves;
        movelist_found = true;
    }

    std::array<Bitboard, 3> colors;
    std::array<Bitboard, 6> pieces;

    Color turn = Color::White;
    std::array<std::array<bool, 2>, 2> can_castle;
    std::array<std::uint8_t, 2> rook_starts;
    std::uint8_t king_start;

    mutable bool movelist_found = false;
    mutable std::vector<Move> movelist;
};

#endif
