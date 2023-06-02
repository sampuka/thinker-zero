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

void Engine::set_position(Position position)
{
    position = position;
}

void Engine::perform_move(const Move& move)
{
    position.perform_move(move);
}