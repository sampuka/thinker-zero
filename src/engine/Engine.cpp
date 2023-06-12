#include "Engine.hpp"

#include "console/uci_output.hpp"
#include "evaluation/evaluation.hpp"
#include "movegen/movegen.hpp"
#include "position/PositionAnalysis.hpp"

#include <iostream>

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

	const float board_evaluation = evaluation::evaluate_board(position);
	std::cout << "Board evaluation: " << board_evaluation << std::endl;

	if (legal_moves.size() == 0)
	{
		uci_bestmove(Move("0000"));
	}
	else
	{
		std::uniform_int_distribution<uint8_t> uid(0, legal_moves.size() - 1);

		uci_bestmove(legal_moves.at(uid(rng)));
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