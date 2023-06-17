#ifndef POSITION_POSITIONSTRING_HPP
#define POSITION_POSITIONSTRING_HPP

#include "position/Position.hpp"

#include <string>

class PositionString
{
public:
	PositionString() = delete;
	PositionString(const std::string& string);  // Takes a FEN string (at least that is the plan)

	Position get_position() const;

private:
	Position m_position;
};

#endif  // POSITION_POSITIONSTRING_HPP
