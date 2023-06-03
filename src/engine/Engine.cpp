#include "Engine.hpp"

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