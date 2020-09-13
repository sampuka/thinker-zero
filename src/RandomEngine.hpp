#ifndef RANDOM_ENGINE_HPP
#define RANDOM_ENGINE_HPP

#include "UCIEngine.hpp"

class RandomEngine : public UCIEngine
{
public:
    RandomEngine()
    {
        start();
    }

    void think() override
    {
        std::vector<Move> moves = board.getMoves();

        std::uniform_int_distribution<std::uint8_t> dist(0, moves.size()-1);

        bestmove = moves.at(dist(eng));
        thinking = false;
    }
};

#endif
