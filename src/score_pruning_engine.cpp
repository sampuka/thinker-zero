#include "UCIEngine.hpp"

#include <functional>

class ScorePruningEngine : public UCIEngine
{
public:
    ScorePruningEngine()
    {
        engine_name = "Score Pruning Engine";
        engine_author = "Mathias Ebbensgaard Jensen";

        start();
    }

    void think() override
    {
        std::uint64_t evals = 0;
        // Default white
        int turn = 1;
        if(board.get_turn() == Color::Black)
            turn = -1;

        MoveList moves = board.get_moves();

        if (moves.size() == 0)
        {
            std::cerr << "No legal moves found!" << std::endl;
        }
        else
        {
            //std::uint8_t size = moves.size();
            //for (std::uint8_t i = 0; i < size; i++)
            for (const Move& move : moves)
                log << move.longform() << std::endl;
        }

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
                evals++;
                //base.evaluation = base.board.adv_eval();
            }

            return;
        };

        const std::function<void(BoardTree&, std::uint8_t)> score_deepening = [&](BoardTree &base, std::uint8_t depth)
        {
            if (base.expanded && base.nodes.size() == 0)
            {
                base.score = -10000; // No reason to deepen a checkmate
                base.evaluation = base.board.adv_eval();
                return;
            }

            if (base.expanded)
            {
                double highest_score = -1000000;
                std::uint8_t node_to_deepen = 0;

                for (std::uint8_t i = 0; i < base.nodes.size(); i++)
                {
                    if (base.nodes.at(i).score > highest_score)
                    {
                        highest_score = base.nodes.at(i).score;
                        node_to_deepen = i;
                    }
                }

                score_deepening(base.nodes.at(node_to_deepen), depth+1);
            }
            else
            {
                base.expand();

                for (BoardTree& node : base.nodes)
                {
                    node.evaluation = node.board.adv_eval();
                    node.score = -3*depth - node.evaluation*turn;

                    if (node.board.typetohere == MoveType::Capture)
                        node.score += 10;
                }
            }

            double highest_score = -1000000;

            for (BoardTree& node : base.nodes)
            {
                if (node.score > highest_score)
                    highest_score = node.score;
            }

            base.score = highest_score;

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
        };

        // Create tree structure
        BoardTree root(board);

        char x = '!';
        //while (time_spent < 10000 && root.score > -100) // Always think for x millisec
        while(x != 'q')
        {
            score_deepening(root, 0);
            std::cout << root.score << std::endl;
            std::cin >> x;
        }

        //minimax(root);

        std::cout << evals << std::endl;

        bestmove = root.bestmove;
        evaluation = root.evaluation*turn;

        // End of function
        thinking = false;
    }
};

int main()
{
    ScorePruningEngine engine;

    return 0;
}
