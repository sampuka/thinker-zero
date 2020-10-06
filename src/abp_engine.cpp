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
        // Default white 
        int turn = 1;
        if(board.get_turn() == Color::Black)
            turn = -1;

        // Get possible moves
        std::vector<Move> moves = board.get_moves();
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
        std::shuffle(moves.begin(), moves.end(), eng);
        bestmove = moves.at(0);

        // Test board
        Board test_board(board);
        Board test_boardTwo(board);

        test_board.perform_move(moves.at(0));

        // Initiate max values
        double bestvalue = -1000000*turn;
        double worstvalue = 1000000*turn;

        for (const Move &moveOne : moves)
        {
            test_board = board;
            test_board.perform_move(moveOne);

            std::vector<Move> movesTwo = test_board.get_moves();

            Move worstmove = movesTwo.at(0);

            // Find best opponent move
            for (const Move &moveTwo : movesTwo)
            {
                test_boardTwo = test_board;
                test_boardTwo.perform_move(moveTwo);

                double bad_eval = test_boardTwo.basic_eval() * turn;
                std::cout << "Bad eval: " << bad_eval << std::endl;

                if(bad_eval <= worstvalue)
                {
                    worstvalue = bad_eval;
                    worstmove = moveTwo;
                }
            }

            test_board.perform_move(worstmove);
            double eval = test_boardTwo.basic_eval() * turn;
            std::cout << "Good eval: " << eval << std::endl;

            if(eval >= bestvalue)
            {
                bestvalue = eval;
                bestmove = moveOne;
            }
        }
        std::cout << "Best value: " << bestvalue << std::endl;
        
        // End of function
        thinking = false;
    }
};

int main()
{
    RandomEngine engine;

    return 0;
}
