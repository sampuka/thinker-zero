#ifndef MOVEGEN_MOVEGEN_RAYS_HPP
#define MOVEGEN_MOVEGEN_RAYS_HPP

#include "types/Bitboard.hpp"
#include "types/Square.hpp"

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

constexpr uint8_t abs_distance(uint8_t a, uint8_t b)
{
    return (a > b) ? (a - b) : (b - a);
}

constexpr std::array<std::array<Bitboard, 64>, 12> movegen_rays = []()
{
    std::array<std::array<Bitboard, 64>, 12> rays;

    for (uint8_t _r = 0; _r < 12; _r++)
    {
        const Ray ray = static_cast<Ray>(_r);

        for (uint8_t index = 0; index < 64; index++)
        {
            const Square from_square(index);

            const uint8_t file = from_square.get_file();
            const uint8_t rank = from_square.get_rank();

            auto generate_by_ray = [&] (uint8_t file_dir, uint8_t rank_dir)
            {
                uint8_t new_file = file + file_dir;
                uint8_t new_rank = rank + rank_dir;

                while (new_file >= FILE_A && new_file <= FILE_H && new_rank >= RANK_1 && new_rank <= RANK_8)
                {
                    Square to_square(new_file, new_rank);

                    rays[_r][index].set_by_square(to_square);

                    new_file += file_dir;
                    new_rank += rank_dir;
                }
            };

            switch (ray)
            {
                case Ray::NE:
                {
                    generate_by_ray(1, 1);
                    break;
                }

                case Ray::E:
                {
                    generate_by_ray(1, 0);
                    break;
                }

                case Ray::SE:
                {
                    generate_by_ray(1, -1);
                    break;
                }

                case Ray::S:
                {
                    generate_by_ray(0, -1);
                    break;
                }

                case Ray::SW:
                {
                    generate_by_ray(-1, -1);
                    break;
                }

                case Ray::W:
                {
                    generate_by_ray(-1, 0);
                    break;
                }

                case Ray::NW:
                {
                    generate_by_ray(-1, 1);
                    break;
                }

                case Ray::N:
                {
                    generate_by_ray(0, 1);
                    break;
                }

                case Ray::WhitePawnAttacks:
                {
                    if (rank != RANK_1 && rank != RANK_8)
                    {
                        if (file != FILE_A)
                        {
                            Square to_square(file - 1, rank + 1);
                            rays[_r][index].set_by_square(to_square);
                        }
                        if (file != FILE_H)
                        {
                            Square to_square(file + 1, rank + 1);
                            rays[_r][index].set_by_square(to_square);
                        }
                    }
                    break;
                }

                case Ray::BlackPawnAttacks:
                {
                    if (rank != RANK_1 && rank != RANK_8)
                    {
                        if (file != FILE_A)
                        {
                            Square to_square(file - 1, rank - 1);
                            rays[_r][index].set_by_square(to_square);
                        }
                        if (file != FILE_H)
                        {
                            Square to_square(file + 1, rank - 1);
                            rays[_r][index].set_by_square(to_square);
                        }
                    }
                    break;
                }

                case Ray::Knight:
                {
                    uint8_t min_file = (file <= FILE_B) ? FILE_A : (file - 2);
                    uint8_t max_file = (file >= FILE_G) ? FILE_H : (file + 2);

                    uint8_t min_rank = (rank <= RANK_2) ? RANK_1 : (rank - 2);
                    uint8_t max_rank = (rank >= RANK_7) ? RANK_8 : (rank + 2);
                    
                    for (uint8_t to_file = min_file; to_file <= max_file; to_file++)
                    {
                        for (uint8_t to_rank = min_rank; to_rank <= max_rank; to_rank++)
                        {
                            Square to_square(to_file, to_rank);

                            uint8_t distance = abs_distance(file, to_file) + abs_distance(rank, to_rank);

                            if (distance == 3)
                            {
                                rays[_r][index].set_by_square(to_square);
                            }
                        }
                    }
                    break;
                }

                case Ray::King:
                {
                    uint8_t min_file = (file == FILE_A) ? FILE_A : (file - 1);
                    uint8_t max_file = (file == FILE_H) ? FILE_H : (file + 1);

                    uint8_t min_rank = (rank == RANK_1) ? RANK_1 : (rank - 1);
                    uint8_t max_rank = (rank == RANK_8) ? RANK_8 : (rank + 1);
                    
                    for (uint8_t to_file = min_file; to_file <= max_file; to_file++)
                    {
                        for (uint8_t to_rank = min_rank; to_rank <= max_rank; to_rank++)
                        {
                            Square to_square(to_file, to_rank);

                            if (from_square.get_data() != to_square.get_data())
                            {
                                rays[_r][index].set_by_square(to_square);
                            }
                        }
                    }
                    break;
                }
            }
        }
    }

    return rays;
}();

#endif // MOVEGEN_MOVEGEN_RAYS_HPP
