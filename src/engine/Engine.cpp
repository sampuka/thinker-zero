#include "Engine.hpp"

#include "console/uci_output.hpp"
#include "movegen/movegen.hpp"

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
    MoveList moves;
    generate_moves<Piece::Pawn>(position, moves);
    generate_moves<Piece::King>(position, moves);

    if (moves.size() == 0)
    {
        uci_bestmove(Move("0000"));
    }
    else
    {
        std::uniform_int_distribution<uint8_t> uid(0, moves.size()-1);

        uci_bestmove(moves.at(uid(rng)));
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