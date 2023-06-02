#ifndef TYPES_POSITIONSTRING_HPP
#define TYPES_POSITIONSTRING_HPP

#include "types/Position.hpp"

#include <string>

class PositionString
{
public:
    PositionString() = delete;
    PositionString(const std::string& string); // Takes a FEN string (at least that is the plan)

    Position get_position() const;

private:
    Position position;
};

#endif // TYPES_POSITIONSTRING_HPP
