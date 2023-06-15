#include "uci_output.hpp"

#include "util/converting.hpp"

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
	std::string str = converting::convert_move_to_string(move);
	std::printf("bestmove %s\n", str.c_str());
}