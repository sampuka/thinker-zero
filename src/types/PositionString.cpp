#include "PositionString.hpp"

PositionString::PositionString(const std::string& string)
{
    (void)string;
    // Parse string -> construct position
}

Position PositionString::get_position() const
{
    return position;
}