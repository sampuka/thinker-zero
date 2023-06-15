#include "Engine.hpp"

#include "console/uci_output.hpp"
#include "movegen/movegen.hpp"
#include "position/PositionAnalysis.hpp"
#include "search/Search.hpp"
#include "util/converting.hpp"

#include <iostream>
#include <string>

Engine::Engine() : rng(rd())
{
}

void Engine::main_loop()
{
	while (engine_running)
	{
		;
	}
}

void Engine::shutdown()
{
	engine_running = false;
}

void Engine::new_game()
{
}

void Engine::go()
{
	MoveList legal_moves = generate_legal_moves(position);

	if (legal_moves.size() == 0)
	{
		uci_bestmove(Move("0000"));
	}
	else
	{
		Search search;
		const std::vector<unsigned int> indices_for_move = search.search_for_best_move(position, legal_moves, 2);
		std::uniform_int_distribution<uint8_t> uid(0, indices_for_move.size() - 1);
		const unsigned int index_for_move = uid(rng);
		const Move move = legal_moves.at(index_for_move);
		std::cout << "Index of move: " << std::to_string(index_for_move) << " (" << converting::convert_move_to_string(move) << ")" << std::endl;
		uci_bestmove(move);
	}
}

void Engine::set_position(Position new_position)
{
	position = new_position;
}

Position& Engine::get_position()
{
	return position;
}

void Engine::perform_move(const Move& move)
{
	position.make_move(move);
}