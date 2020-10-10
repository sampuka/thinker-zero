#include "UCIEngine.hpp"

class ABPEngine : public UCIEngine
{
public:
    ABPEngine()
    {
        engine_name = "ABP Engine";
        engine_author = "Mathias Lyngbye and Martin Jakobsgaard";

        start();
    }

    void think() override
    {
        // Default white
        int turn = 1;
        if(board.get_turn() == Color::Black)
            turn = -1;

        // Get possible moves
        MoveList& moves = board.get_moves();

        if (moves.size() == 0)
        {
            std::cerr << "No legal moves found!" << std::endl;
        }
        else
        {
            std::uint8_t size = moves.size();
            for (std::uint8_t i = 0; i < size; i++)
                log << moves.at(i).longform() << std::endl;
        }

        // Find random move among moves
        std::shuffle(moves.begin(), moves.end(), eng);
        bestmove = moves.at(0);

        std::ofstream foobar("/tmp/foobar.txt");

        // Initiate max values
        double bestvalue = -1000000;

        for (const Move &moveOne : moves)
        {
            foobar << moveOne.longform() << " :" << std::endl;
            Board test_board = board;
            test_board.perform_move(moveOne);

            MoveList& movesTwo = test_board.get_moves();

            double eval = 0;

            if (movesTwo.size() != 0)
            {
                Move worstmove = Move(0,0,0,0);
                double worstvalue = 1000000;

                // Find best opponent move
                for (const Move &moveTwo : movesTwo)
                {
                    Board test_boardTwo = test_board;
                    test_boardTwo.perform_move(moveTwo);

                    double bad_eval = test_boardTwo.basic_eval() * turn;

                    foobar << "\t" << moveTwo.longform() << " " << bad_eval << " " << worstvalue << std::endl;

                    if(bad_eval <= worstvalue)
                    {
                        worstvalue = bad_eval;
                        worstmove = moveTwo;
                    }
                }

                eval = worstvalue;
            }
            else
            {
                if (test_board.is_stalemate())
                    eval = 0;
                else if (test_board.is_checkmate())
                    eval = 200;
            }

            foobar << "\t\t = " << eval << std::endl;

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
    ABPEngine engine;

    return 0;
}
