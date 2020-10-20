#include "UCIEngine.hpp"

#include <functional>
#include <cmath>

class MinimaxEngine : public UCIEngine
{
public:
    MinimaxEngine()
    {
        start();
    }

    MoveList movelist;

    void think() override
    {
        // Default white
        int turn_bias = 1;
        if(board.get_turn() == Color::Black)
            turn_bias = -1;

        MoveList startmoves;
        board.get_moves(startmoves);

        bestmove = startmoves.at(1);

        /*
        // Create root
        BoardTree root(board);

        // Selection
        BoardTree node = root;
        while(true)
        {
            // Break if leaf node
            if(node.nodes.size() == 0)
                break;

            // Find best child of node
            double best_child_evaluation = -999999;
            BoardTree best_child;
            for (const BoardTree& child : node.nodes)
            {
                if((child.evaluation*turn_bias) > best_child_evaluation)
                {
                    best_child_evalutaion = child.evaluation;
                    best_child = child;
                }
            }

            // Select best child
        }
        */

        // End of function
        thinking = false;
    }

    double uct(int parent_visit, int node_visit, double node_score, double c)
    {
        return ((node_score / node_visit) + c * std::sqrt(std::log(parent_visit) / node_visit));
    }
};

int main()
{
    MinimaxEngine engine;

    return 0;
}
