#ifndef ENGINE_ENGINE_HPP
#define ENGINE_ENGINE_HPP

#include "types/Move.hpp"
#include "types/Position.hpp"

class Engine
{
public:
    Engine() = default;

    // Practical stuff
    void shutdown();
    void main_loop();

    // Engine stuff
    void new_game();

    // Chess stuff
    void set_position(Position new_position);
    Position& get_position();

    void perform_move(const Move& move);

private:
    // Practical stuff
    bool engine_running = true;

    // Engine stuff

    // Chess stuff
    Position position;
};

inline Engine engine;

#endif // ENGINE_ENGINE_HPP
