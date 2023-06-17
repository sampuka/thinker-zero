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

	void perft(uint8_t depth);

	// Chess stuff
	void set_position(Position new_position);
	Position& get_position();

	void perform_move(const Move& move);

private:
	// Practical stuff
	bool m_engine_running = true;
	std::random_device m_random_device;
	std::mt19937 m_rng;

	// Engine stuff
	uint64_t perft_layer(const Position& perft_position, uint8_t depth);

	// Chess stuff
	Position m_position;
};

inline Engine engine;

#endif  // ENGINE_ENGINE_HPP
