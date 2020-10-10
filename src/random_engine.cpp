#include "UCIEngine.hpp"

class RandomEngine : public UCIEngine
{
public:
    RandomEngine()
    {
        engine_name = "Random Engine";
        engine_author = "Mathias Ebbensgaard Jensen";

        start();
    }

    void think() override
    {
        MoveList& moves = board.get_moves();

        if (moves.size() == 0)
        {
            std::cerr << "No legal moves found!" << std::endl;
        }
        else
        {
            for (const Move &move : moves)
                log << move.longform() << std::endl;
        }

        std::uniform_int_distribution<std::uint8_t> dist(0, moves.size()-1);

        bestmove = moves.at(dist(eng));
        thinking = false;
    }
};

int main()
{
    RandomEngine engine;

    return 0;
}
