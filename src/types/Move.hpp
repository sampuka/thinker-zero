#ifndef TYPES_MOVE_HPP
#define TYPES_MOVE_HPP

#include "types/Square.hpp"

#include <cstdint>
#include <string>

enum class MoveType : uint8_t
{
    Quiet = 0,
    Reserved1 = 1,
    Reserved2 = 2,
    Reserved3 = 3,
    Reserved4 = 4,
    Reserved5 = 5,
    Reserved6 = 6,
    Reserved7 = 7,
    KnightPromo = 8,
    BishopPromo = 9,
    RookPromo = 10,
    QueenPromo = 11,
    Reserved12 = 12,
    Reserved13 = 13,
    Reserved14 = 14,
    Reserved15 = 15
};

class Move
{
public:
    Move() = default;
    Move(const std::string& move_string); // Construct the move on an empty board from a move string. "e7e8q", for instance
    Move(Square from, Square to);
    //Move(const Position& position, const std::string& move_string); // Construct the move on a certain board.

    void set_to_square(Square to_square);
    Square get_to_square() const;

    void set_from_square(Square from_square);
    Square get_from_square() const;

    void set_type(MoveType type);
    MoveType get_type() const;

private:
    uint16_t encoded_move = 0;
};

#endif // TYPES_MOVE_HPP
