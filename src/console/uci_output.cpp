#include "uci_output.hpp"

#include "engine/Settings.hpp"

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
	std::printf("%s", engine_settings.get_uci_string().c_str());

	std::printf("uciok\n");
}

void uci_bestmove(const Move& move)
{
	std::string str = move.get_string();
	std::printf("bestmove %s\n", str.c_str());
}