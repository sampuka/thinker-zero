#ifndef UTIL_CONVERTING_HPP
#define UTIL_CONVERTING_HPP

#pragma once
#include "movegen/movegen.hpp"

#include <string>
namespace converting
{

static const std::string convert_move_to_string(const Move& move)
{
	Square from = move.get_from_square();
	Square to = move.get_to_square();

	std::string str;
	str += static_cast<char>(from.get_file() + 'a' - 1);
	str += static_cast<char>(from.get_rank() + '0');
	str += static_cast<char>(to.get_file() + 'a' - 1);
	str += static_cast<char>(to.get_rank() + '0');

	switch (move.get_type())
	{
		case MoveType::KnightPromo:
		{
			str += "n";
			break;
		}
		case MoveType::BishopPromo:
		{
			str += "b";
			break;
		}
		case MoveType::RookPromo:
		{
			str += "r";
			break;
		}
		case MoveType::QueenPromo:
		{
			str += "q";
			break;
		}
		default:
		{
			break;
		}
	}

	return str;
}
}  // namespace converting

#endif  // UTIL_CONVERTING_HPP
