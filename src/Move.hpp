#ifndef MOVE_HPP
#define MOVE_HPP

#include "utility.hpp"

#include <array>
#include <cstdint>
#include <string>

enum class MoveType
{
    Quiet = 0,
    Capture,
    Evasion,
    EnPassant,
    Castling
};

enum class MoveSpecial
{
    None = 0,
    Promotion,
    EnPassant,
    Castling
};

class Move
{
public:
    Move()
        : data(0), fx(0), fy(0), tx(0), ty(0), promo(Piece::None)
    {}

    /*
    Move(const Move &m)
        : data(m.data), fx(m.fx), fy(m.fy), tx(m.tx), ty(m.ty), promo(m.promo)
    {}
    */

    // From x, from y, to x, to y
    Move(std::uint8_t fx_, std::uint8_t fy_, std::uint8_t tx_, std::uint8_t ty_, Piece promo_ = Piece::None)
        : fx(fx_), fy(fy_), tx(tx_), ty(ty_), promo(promo_)
    {
        set_from(fy*8+fx);
        set_to(ty*8+tx);
        if (promo != Piece::None)
            set_type(MoveSpecial::Promotion);
        else
            set_type(MoveSpecial::None);
        set_promo(promo);
    }

    Move(Square from, Square to, MoveSpecial type, Piece promo_ = Piece::None)
    {
        set_from(from);
        set_to(to);
        set_type(type);
        set_promo(promo_);
    }

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
                case 'n': promo = Piece::Knight; break;
                case 'b': promo = Piece::Bishop; break;
                case 'r': promo = Piece::Rook;   break;
                case 'q': promo = Piece::Queen;  break;
                default: break;
            }
        }

        set_from(fy*8+fx);
        set_to(ty*8+tx);

        if (promo != Piece::None)
            set_type(MoveSpecial::Promotion);
        else
            set_type(MoveSpecial::None);
        set_promo(promo);
    }

    void set_from(Square from)
    {
        data &= std::uint16_t{0b0000001111111111};
        data |= static_cast<std::uint16_t>(from) << 10;
    }

    Square get_from() const
    {
        return static_cast<Square>((data & std::uint16_t{0b1111110000000000}) >> 10);
    }

    void set_to(Square to)
    {
        data &= std::uint16_t{0b1111110000001111};
        data |= static_cast<std::uint16_t>(to) << 4;
    }

    Square get_to() const
    {
        return static_cast<Square>((data & std::uint16_t{0b0000001111110000}) >> 4);
    }

    void set_type(MoveSpecial type)
    {
        data &= std::uint16_t{0b1111111111110011};
        data |= static_cast<std::uint16_t>(type) << 2;
    }

    MoveSpecial get_type() const
    {
        return static_cast<MoveSpecial>((data & std::uint16_t{0b0000000000001100}) >> 2);
    }

    void set_promo(Piece promo_)
    {
        if ((promo_ == Piece::None) || (promo_ == Piece::Pawn) || (promo_ == Piece::King))
            promo_ = Piece::Knight;

        data &= std::uint16_t{0b1111111111111100};
        data |= (static_cast<std::uint16_t>(promo_)-static_cast<std::uint16_t>(Piece::Knight));
    }

    Piece get_promo() const
    {
        if (get_type() == MoveSpecial::Promotion)
            return static_cast<Piece>((data & std::uint16_t{0b0000000000000011}) + static_cast<std::uint16_t>(Piece::Knight));
        else
            return Piece::None;
    }

    std::string longform() const
    {
        if (fx == 0 && fy == 0 && tx == 0 && ty == 0)
            return "0000";

        std::string s;

        /*
        s += static_cast<char>('a'+get_from()%8);
        s += static_cast<char>('1'+get_from()/8);
        s += static_cast<char>('a'+get_to()%8);
        s += static_cast<char>('1'+get_to()/8);

        if (get_type() == MoveSpecial::Promotion)
        {
            switch (get_promo())
            {
                case Piece::Knight: s += 'n'; break;
                case Piece::Bishop: s += 'b'; break;
                case Piece::Rook:   s += 'r'; break;
                case Piece::Queen:  s += 'q'; break;
                default: s += '?'; break;
            }
        }
        */

        s += static_cast<char>('a'+fx);
        s += static_cast<char>('1'+fy);
        s += static_cast<char>('a'+tx);
        s += static_cast<char>('1'+ty);

        switch (promo)
        {
            case Piece::None: break;
            case Piece::Knight: s += 'n'; break;
            case Piece::Bishop: s += 'b'; break;
            case Piece::Rook:   s += 'r'; break;
            case Piece::Queen:  s += 'q'; break;
            default: s += '?'; break;
        }

        return s;
    }

    void print() const
    {
        std::string s;

        if (fx == 0 && fy == 0 && tx == 0 && ty == 0)
            s = "0000";

        /*
        s += static_cast<char>('a'+get_from()%8);
        s += static_cast<char>('1'+get_from()/8);
        s += static_cast<char>('a'+get_to()%8);
        s += static_cast<char>('1'+get_to()/8);

        if (get_type() == MoveSpecial::Promotion)
        {
            switch (get_promo())
            {
                case Piece::Knight: s += 'n'; break;
                case Piece::Bishop: s += 'b'; break;
                case Piece::Rook:   s += 'r'; break;
                case Piece::Queen:  s += 'q'; break;
                default: s += '?'; break;
            }
        }
        */

        s += static_cast<char>('a'+fx);
        s += static_cast<char>('1'+fy);
        s += static_cast<char>('a'+tx);
        s += static_cast<char>('1'+ty);

        switch (promo)
        {
            case Piece::None: break;
            case Piece::Knight: s += 'n'; break;
            case Piece::Bishop: s += 'b'; break;
            case Piece::Rook:   s += 'r'; break;
            case Piece::Queen:  s += 'q'; break;
            default: s += '?'; break;
        }

        switch(get_type())
        {
            case MoveSpecial::None: s+= "(none)"; break;
            case MoveSpecial::Promotion: s+= "(promotion)"; break;
            case MoveSpecial::EnPassant: s+= "(en passant)"; break;
            case MoveSpecial::Castling: s+= "(castling)"; break;
        }

        std::cout << s << std::endl;
    }

    // Bit (0:5): from square
    // Bit (6:11) to square
    // Bit (12:13) promotion piece (0 = knight, 1 = bishop, 2 = rook, 3 = queen)
    // Bit (14:15) move type (0 = promotion, 1 = en passant, 2 = castling)
    std::uint16_t data = 0;

private:
    std::uint8_t fx = 0;
    std::uint8_t fy = 0;
    std::uint8_t tx = 0;
    std::uint8_t ty = 0;
    Piece promo = Piece::None;
};

class MoveList
{
    public:
        std::uint8_t size() const
        {
            return list_size;
        }

        Move at(std::uint8_t i) const
        {
            return list[i];
        }

        void add_move(const Move &m)
        {
            list[list_size++] = m;
        }

        void clear()
        {
            list_size = 0;
        }

        Move* begin()
        {
            return std::begin(list);
        }

        Move* end()
        {
            return std::begin(list)+list_size;
    }

private:
    std::uint8_t list_size = 0;
    std::array<Move, 256> list;
};

#endif
