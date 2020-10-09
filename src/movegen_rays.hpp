#ifndef MOVEGEN_RAYS_HPP
#define MOVEGEN_RAYS_HPP

#include "Bitboard.hpp"

#include <array>
#include <functional>

enum class Ray
{
    NE = 0,
    E,
    SE,
    S,
    SW,
    W,
    NW,
    N,
    WhitePawnAttacks,
    BlackPawnAttacks,
    Knight,
    King
};

const std::array<std::array<Bitboard, 64>, 12> movegen_rays = []()
{
    std::array<std::array<Bitboard, 64>, 12> rays;

    for (std::uint8_t _r = 0; _r < 12; _r++)
    {
        const Ray ray = static_cast<Ray>(_r);

        for (std::uint8_t index = 0; index < 64; index++)
        {
            std::uint8_t x = index%8;
            std::uint8_t y = index/8;

            switch (ray)
            {
                case Ray::NE:
                    x++; y++;
                    while ((x < 8) && (y < 8))
                    {
                        bitboard_set(rays[_r][index], x, y);
                        x++; y++;
                    }
                    break;

                case Ray::E:
                    x++;
                    while ((x < 8) && (y < 8))
                    {
                        bitboard_set(rays[_r][index], x, y);
                        x++;
                    }
                    break;

                case Ray::SE:
                    x++; y--;
                    while ((x < 8) && (y < 8))
                    {
                        bitboard_set(rays[_r][index], x, y);
                        x++; y--;
                    }
                    break;

                case Ray::S:
                    y--;
                    while ((x < 8) && (y < 8))
                    {
                        bitboard_set(rays[_r][index], x, y);
                        y--;
                    }
                    break;

                case Ray::SW:
                    x--; y--;
                    while ((x < 8) && (y < 8))
                    {
                        bitboard_set(rays[_r][index], x, y);
                        x--; y--;
                    }
                    break;

                case Ray::W:
                    x--;
                    while ((x < 8) && (y < 8))
                    {
                        bitboard_set(rays[_r][index], x, y);
                        x--;
                    }
                    break;

                case Ray::NW:
                    x--; y++;
                    while ((x < 8) && (y < 8))
                    {
                        bitboard_set(rays[_r][index], x, y);
                        x--; y++;
                    }
                    break;

                case Ray::N:
                    y++;
                    while ((x < 8) && (y < 8))
                    {
                        bitboard_set(rays[_r][index], x, y);
                        y++;
                    }
                    break;

                case Ray::WhitePawnAttacks:
                    if (y != 0 && y != 7)
                    {
                        if (x > 0)
                            bitboard_set(rays[_r][index], x-1, y+1);
                        if (x < 7)
                            bitboard_set(rays[_r][index], x+1, y+1);
                    }
                    break;

                case Ray::BlackPawnAttacks:
                    if (y != 0 && y != 7)
                    {
                        if (x > 0)
                            bitboard_set(rays[_r][index], x-1, y-1);
                        if (x < 7)
                            bitboard_set(rays[_r][index], x+1, y-1);
                    }
                    break;

                case Ray::Knight:
                    for (std::uint8_t i = 0; i < 8; i++)
                    {
                        std::int8_t x_ = x+km(i,0);
                        std::int8_t y_ = y+km(i,1);

                        if ((x_ >= 0) && (y_ >= 0) && (x_ < 8) && (y_ < 8))
                        {
                            bitboard_set(rays[_r][index], x_, y_);
                        }
                    }
                    break;

                case Ray::King:
                    for (std::int8_t x_ = x-1; x_ <= x+1; x_++)
                    {
                        for (std::int8_t y_ = y-1; y_ <= y+1; y_++)
                        {
                            if ((x_ >= 0) && (y_ >= 0) && (x_ < 8) && (y_ < 8) && (!(x_ == x && y_ == y)))
                            {
                                bitboard_set(rays[_r][index], x_, y_);
                            }
                        }
                    }
                    break;
            }
        }
    }

    return rays;
}();

// White king side castle clear squares
const Bitboard wks_clear = []()
{
    return 0;
}();

const Bitboard wks_safe = []()
{
    return 0;
}();

#endif
