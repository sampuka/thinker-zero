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
        : data(0)
    {}

    // From x, from y, to x, to y
    Move(std::uint8_t fx, std::uint8_t fy, std::uint8_t tx, std::uint8_t ty, Piece promo = Piece::None)
    {
        set_from(fy*8+fx);
        set_to(ty*8+tx);
        if (promo != Piece::None)
            set_type(MoveSpecial::Promotion);
        else
            set_type(MoveSpecial::None);
        set_promo(promo);
    }

    Move(Square from, Square to, MoveSpecial type, Piece promo = Piece::None)
    {
        set_from(from);
        set_to(to);
        set_type(type);
        set_promo(promo);
    }

    Move(std::string s)
    {
        std::uint8_t fx = static_cast<std::uint8_t>(s.at(0)-'a');
        std::uint8_t fy = static_cast<std::uint8_t>(s.at(1)-'1');
        std::uint8_t tx = static_cast<std::uint8_t>(s.at(2)-'a');
        std::uint8_t ty = static_cast<std::uint8_t>(s.at(3)-'1');

        if (s.size() == 5)
        {
            switch (s.at(4))
            {
                case 'n': set_promo(Piece::Knight); break;
                case 'b': set_promo(Piece::Bishop); break;
                case 'r': set_promo(Piece::Rook);   break;
                case 'q': set_promo(Piece::Queen);  break;
                default: break;
            }
            set_type(MoveSpecial::Promotion);
        }
        else
        {
            set_type(MoveSpecial::None);
        }

        set_from(fy*8+fx);
        set_to(ty*8+tx);
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
        if (get_from() == 0 && get_to() == 0)
            return "0000";

        std::string s;

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

        /*
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
        */

        return s;
    }

    void print() const
    {
        std::string s;

        if (get_from() == 0 && get_to() == 0)
            s = "0000";

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
    // Bit (14:15) move type (0 = none, 1 = promotion, 2 = en passant, 3 = castling)
    std::uint16_t data = 0;
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
    std::array<Move, 100> list;
};

#endif
