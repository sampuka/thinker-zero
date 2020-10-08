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

constexpr std::array<std::array<Bitboard, 64>, 12> movegen_rays = []()
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
                        rays[_r][index].write(x, y, true);
                        x++; y++;
                    }
                    break;

                case Ray::E:
                    x++;
                    while ((x < 8) && (y < 8))
                    {
                        rays[_r][index].write(x, y, true);
                        x++;
                    }
                    break;

                case Ray::SE:
                    x++; y--;
                    while ((x < 8) && (y < 8))
                    {
                        rays[_r][index].write(x, y, true);
                        x++; y--;
                    }
                    break;

                case Ray::S:
                    y--;
                    while ((x < 8) && (y < 8))
                    {
                        rays[_r][index].write(x, y, true);
                        y--;
                    }
                    break;

                case Ray::SW:
                    x--; y--;
                    while ((x < 8) && (y < 8))
                    {
                        rays[_r][index].write(x, y, true);
                        x--; y--;
                    }
                    break;

                case Ray::W:
                    x--;
                    while ((x < 8) && (y < 8))
                    {
                        rays[_r][index].write(x, y, true);
                        x--;
                    }
                    break;

                case Ray::NW:
                    x--; y++;
                    while ((x < 8) && (y < 8))
                    {
                        rays[_r][index].write(x, y, true);
                        x--; y++;
                    }
                    break;

                case Ray::N:
                    y++;
                    while ((x < 8) && (y < 8))
                    {
                        rays[_r][index].write(x, y, true);
                        y++;
                    }
                    break;

                case Ray::WhitePawnAttacks:
                    if (y != 0 && y != 7)
                    {
                        if (x > 0)
                            rays[_r][index].write(x-1, y+1, true);
                        if (x < 7)
                            rays[_r][index].write(x+1, y+1, true);
                    }
                    break;

                case Ray::BlackPawnAttacks:
                    if (y != 0 && y != 7)
                    {
                        if (x > 0)
                            rays[_r][index].write(x-1, y-1, true);
                        if (x < 7)
                            rays[_r][index].write(x+1, y-1, true);
                    }
                    break;

                case Ray::Knight:
                    for (std::uint8_t i = 0; i < 8; i++)
                    {
                        std::int8_t x_ = x+km(i,0);
                        std::int8_t y_ = y+km(i,1);

                        if ((x_ >= 0) && (y_ >= 0) && (x_ < 8) && (y_ < 8))
                        {
                            rays[_r][index].write(x_, y_, true);
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
                                rays[_r][index].write(x_, y_, true);
                            }
                        }
                    }
                    break;
            }
        }
    }

    return rays;
}();

#endif
