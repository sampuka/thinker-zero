#ifndef TYPES_SQUARE_HPP
#define TYPES_SQUARE_HPP

#include <cstdint>

class Square
{
public:
    Square() = delete;

    constexpr Square(uint8_t file, uint8_t rank) // Move(3, 2) for c2
    {
        encoded_square = (rank - 1) * 8 + (file - 1);
    }

    constexpr Square(uint8_t encoded_data)
    {
        encoded_square = encoded_data;
    }

    constexpr uint8_t get_data() const
    {
        return encoded_square;
    }

    constexpr uint8_t get_file() const
    {
        return (encoded_square % 8) + 1;
    }

    constexpr uint8_t get_rank() const
    { 
        return (encoded_square / 8) + 1;
    }

private:
    uint8_t encoded_square;
};

constexpr uint8_t FILE_A = 1;
constexpr uint8_t FILE_B = 2;
constexpr uint8_t FILE_C = 3;
constexpr uint8_t FILE_D = 4;
constexpr uint8_t FILE_E = 5;
constexpr uint8_t FILE_F = 6;
constexpr uint8_t FILE_G = 7;
constexpr uint8_t FILE_H = 8;

constexpr uint8_t RANK_1 = 1;
constexpr uint8_t RANK_2 = 2;
constexpr uint8_t RANK_3 = 3;
constexpr uint8_t RANK_4 = 4;
constexpr uint8_t RANK_5 = 5;
constexpr uint8_t RANK_6 = 6;
constexpr uint8_t RANK_7 = 7;
constexpr uint8_t RANK_8 = 8;

#endif // TYPES_SQUARE_HPP
