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
        bestmove = startmoves.at(1); // Initialize with something (test)

        // Create root
        BoardTree root(board);

        while(time_spent < 5000)
        {
            ;//std::cout << time_spent << std::endl;
        }

        // End of function
        thinking = false;
    }

    const BoardTree *find_best_child(const BoardTree &node)
    {
        int parent_visitcount = node.visitcount;
        const auto comparator = [&](const BoardTree &a, const BoardTree &b)
        {
            double node_a_uct = uct(parent_visitcount, a.winscore, a.visitcount);
            double node_b_uct = uct(parent_visitcount, b.winscore, b.visitcount);
            return (node_a_uct < node_b_uct);
        };

        return &(*std::max_element(node.nodes.begin(), node.nodes.end(), comparator));
    }

    double uct(int parent_visit, int node_visit, double node_score)
    {
        double c = std::sqrt(2.0); // Theoretical value
        return ((node_score / node_visit) + c * std::sqrt(std::log(parent_visit) / node_visit));
    }
};

int main()
{
    MinimaxEngine engine;

    return 0;
}
