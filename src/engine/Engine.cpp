#include "Engine.hpp"

#include "console/uci_output.hpp"
#include "engine/Settings.hpp"
#include "evaluation/evaluation_type.hpp"
#include "movegen/movegen.hpp"
#include "position/PositionAnalysis.hpp"
#include "search/Search.hpp"

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
	else if (engine_settings.get_random_moves_only())
	{
		std::uniform_int_distribution<uint8_t> uid(0, legal_moves.size() - 1);
		const unsigned int index_for_move = uid(rng);
		const Move move = legal_moves.at(index_for_move);
		uci_bestmove(move);
	}
	else
	{
		Search search(EVALUATION_TYPE::SIMPLIFIED_EVALUATION_FUNCTION);
		const std::vector<unsigned int> indices_for_move = search.search_for_best_move(position, legal_moves, engine_settings.get_max_search_depth());

		if (indices_for_move.empty() == true)
		{
			// No moves to be done.
			uci_bestmove(Move("0000"));
		}
		std::uniform_int_distribution<uint8_t> uid(0, indices_for_move.size() - 1);
		const unsigned int index_for_move = uid(rng);
		const Move move = legal_moves.at(indices_for_move[index_for_move]);
		uci_bestmove(move);
	}
}

void Engine::perft(uint8_t depth)
{
	MoveList legal_moves = generate_legal_moves(position);

	uint64_t total_nodes = 0;

	for (const Move& move : legal_moves)
	{
		Position move_position = position;
		move_position.make_move(move);

		uint64_t nodes = perft_layer(move_position, depth - 1);

		total_nodes += nodes;

		std::printf("%s: %llu\n", move.get_string().c_str(), nodes);
	}

	std::printf("\nNodes searched: %llu\n\n", total_nodes);
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

uint64_t Engine::perft_layer(const Position& perft_position, uint8_t depth)
{
	if (depth == 0)
	{
		return 1;
	}

	uint64_t layer_nodes = 0;

	MoveList legal_moves = generate_legal_moves(perft_position);

	for (const Move& move : legal_moves)
	{
		Position new_position = perft_position;
		new_position.make_move(move);

		layer_nodes += perft_layer(new_position, depth - 1);
	}

	return layer_nodes;
}