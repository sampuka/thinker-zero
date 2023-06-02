#ifndef TYPES_MOVE_HPP
#define TYPES_MOVE_HPP

#include <string>

class Move
{
public:
    Move() = delete;
    Move(const std::string& move_string); // "e7e8q", for instance
};

#endif // TYPES_MOVE_HPP
