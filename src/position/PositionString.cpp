#include "PositionString.hpp"

PositionString::PositionString(const std::string& string)
{
	if (string == "startpos")
	{
		position.setup_standard_position();
	}
	// Parse string -> construct position
}

Position PositionString::get_position() const
{
	return position;
}