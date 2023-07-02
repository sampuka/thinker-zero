#ifndef POSITION_POSITIONANALYSIS_HPP
#define POSITION_POSITIONANALYSIS_HPP

#include "position/Position.hpp"

class PositionAnalysis
{
public:
	PositionAnalysis() = delete;
	PositionAnalysis(const Position& position);

	bool king_in_check() const;
	Bitboard threatened_squares() const;

private:
	const Position m_position;
};

#endif  // POSITION_POSITIONANALYSIS_HPP
