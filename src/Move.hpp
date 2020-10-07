#ifndef MOVE_HPP
#define MOVE_HPP

#include "utility.hpp"

#include <cstdint>
#include <string>

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
                case 'n': promo = Piece::Knight; break;
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
            case Piece::Knight: s += 'n'; break;
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

#endif
