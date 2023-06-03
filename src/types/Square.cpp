#include "Square.hpp"

Square::Square(uint8_t file, uint8_t rank)
{
    encoded_square = (rank - 1) * 8 + (file - 1);
}

Square::Square(uint8_t encoded_data)
{
    encoded_square = encoded_data;
}

uint8_t Square::get_data() const
{
    return encoded_square;
}