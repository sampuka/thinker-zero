#ifndef TYPES_POSITION_HPP
#define TYPES_POSITION_HPP

#include "types/Move.hpp"

class Position
{
public:
    Position() = default;

    void perform_move(const Move& move);
};

#endif // TYPES_POSITION_HPP
