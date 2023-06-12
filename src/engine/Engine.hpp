#ifndef ENGINE_ENGINE_HPP
#define ENGINE_ENGINE_HPP

#include "position/Position.hpp"
#include "types/Move.hpp"

#include <random>


class Engine
{
public:
	Engine();

	// Practical stuff
	void shutdown();
	void main_loop();

	// Engine stuff
	void new_game();

	void go();

	// Chess stuff
	void set_position(Position new_position);
	Position& get_position();

	void perform_move(const Move& move);

private:
	// Practical stuff
	bool engine_running = true;
	std::random_device rd;
	std::mt19937 rng;

	// Engine stuff

	// Chess stuff
	Position position;
};

inline Engine engine;

#endif  // ENGINE_ENGINE_HPP
