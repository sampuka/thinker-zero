#ifndef POSITION_POSITIONANALYSIS_HPP
#define POSITION_POSITIONANALYSIS_HPP

#include "position/Position.hpp"

class PositionAnalysis
{
public:
	PositionAnalysis() = delete;
	PositionAnalysis(const Position& position_);

	bool king_in_check() const;

private:
	const Position position;
};

#endif  // POSITION_POSITIONANALYSIS_HPP
