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
                movelist.is_checkmate = base.is_checkmate;
                movelist.is_stalemate = base.is_stalemate;
                base.evaluation = base.board.adv_eval(movelist);
                /*
                if (movelist.is_stalemate)
                {
                    std::cout << "Evaluating stalemate due to repetition: " << base.evaluation << std::endl;
                }
                */
            }

            return;
        };

        // Create tree structure
        BoardTree root(board);
        root.expand(movelist, z_list, 4);

        minimax(root);

        bestmove = root.bestmove;
        evaluation = root.evaluation*turn;

        /*
        std::cout << "About to move " << bestmove.longform() << std::endl;
        std::cout << root.nodes.size() << std::endl;
        std::cout << evaluation << std::endl;

        MoveList m2;
        BoardTree test(board, bestmove);
        auto z2 = z_list;
        z2.push_back(test.board.get_zobrist());
        test.board.get_moves(m2, z2, false);
        std::cout << int{m2.size()} << std::endl;
        std::cout << m2.is_checkmate << std::endl;
        std::cout << m2.is_stalemate << std::endl;
        std::cout << test.board.adv_eval(m2) << std::endl;
        */

        // End of function
        thinking = false;
    }
};

int main()
{
    MinimaxEngine engine;

    return 0;
}
