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
        // Get possible moves
        std::vector<Move> moves = board.getMoves();
        if (moves.size() == 0)
        {
            std::cerr << "No legal moves found!" << std::endl;
        }
        else
        {
            for (const Move &move : moves)
                log << move.longform() << std::endl;
        }
   
        // Find random move among moves
        std::uniform_int_distribution<std::uint8_t> dist(0, moves.size()-1);
        bestmove = moves.at(dist(eng));
        
        // Create test board
        Board test_board(board);
        test_board.performMove(moves.at(0));
        
        // End of function
        thinking = false;
    }
};

int main()
{
    RandomEngine engine;

    return 0;
}
