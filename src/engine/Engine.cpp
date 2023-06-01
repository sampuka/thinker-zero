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