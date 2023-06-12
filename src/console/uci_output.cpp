#include "uci_output.hpp"

void uci_readyok()
{
	std::printf("readyok\n");
}

void uci_uciok()
{
	std::printf(
		"id name Thinker-zero Chess Engine\n"
		"id author Mathias Ebbensgaard Jensen\n");

	// Send supported settings

	std::printf("uciok\n");
}

void uci_bestmove(const Move& move)
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

	std::printf("bestmove %s\n", str.c_str());
}