#include "UCIEngine.hpp"

#include <functional>

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
        int turn = 1;
        if(board.get_turn() == Color::Black)
            turn = -1;

        std::uniform_int_distribution<std::uint8_t> dist(0,1);

        const std::function<void(BoardTree&)> minimax = [&](BoardTree &base)
        {
            if (base.expanded && (base.nodes.size() != 0))
            {
                for (BoardTree &node : base.nodes)
                {
                    minimax(node);
                }

                if (base.board.get_turn() == Color::White)
                {
                    double eval = -1000000;
                    Move best;

                    for (const BoardTree &node : base.nodes)
                    {
                        if (
                                (node.evaluation > eval) ||
                                (node.evaluation == eval && dist(eng) == 1)
                           )
                        {
                            eval = node.evaluation;
                            best = node.move;
                        }
                    }

                    base.evaluation = eval;
                    base.bestmove = best;
                }
                else
                {
                    double eval = 1000000;
                    Move best;

                    for (const BoardTree &node : base.nodes)
                    {
                        if (
                                (node.evaluation < eval) ||
                                (node.evaluation == eval && dist(eng) == 1)
                           )
                        {
                            eval = node.evaluation;
                            best = node.move;
                        }
                    }

                    base.evaluation = eval;
                    base.bestmove = best;
                }
            }
            else
            {
                base.board.get_moves(movelist);
                base.evaluation = base.board.adv_eval(movelist);
            }

            return;
        };

        // Create tree structure
        BoardTree root(board);
        root.expand(movelist, 4);

        minimax(root);

        bestmove = root.bestmove;
        evaluation = root.evaluation*turn;

        // End of function
        thinking = false;
    }
};

int main()
{
    MinimaxEngine engine;

    return 0;
}
