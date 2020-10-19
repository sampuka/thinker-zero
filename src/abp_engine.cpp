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

    double alphaBetaMax(BoardTree& base, double alpha, double beta, int depthleft)
    {
        if (depthleft == 0)
            return base.board.adv_eval();

        base.expand(1);

        if(base.nodes.size() == 0)
            return base.board.adv_eval();

        for (BoardTree &node : base.nodes)
        {
            double score = alphaBetaMin(node, alpha, beta, depthleft - 1);
            if(score >= beta)
                return beta;   // fail hard beta-cutoff
            if(score > alpha)
                alpha = score; // alpha acts like max in MiniMax
        }
        return alpha;
    }

    double alphaBetaMin(BoardTree& base, double alpha, double beta, int depthleft)
    {
        if (depthleft == 0)
            return base.board.adv_eval();

        base.expand(1);

        if(base.nodes.size() == 0)
            return base.board.adv_eval();

        for (BoardTree &node : base.nodes)
        {
            double score = alphaBetaMax(node, alpha, beta, depthleft - 1);
            if(score <= alpha)
                return alpha; // fail hard alpha-cutoff
            if(score < beta)
                beta = score; // beta acts like min in MiniMax
        }
        return beta;
    }

    /*
    //Look at nodes which involves captures.
    double quiesce(BoardTree& base, double alpha, double beta )
    {
        double stand_pat = base.board.adv_eval();
        if(stand_pat >= beta)
            return beta;
        if(alpha < stand_pat)
            alpha = stand_pat;

        until( every_capture_has_been_examined )
        {
            MakeCapture();
            double score = -Quiesce( -beta, -alpha );
            TakeBackMove();

            if( score >= beta )
                return beta;
            if( score > alpha )
               alpha = score;
        }
        return alpha;
    }
    */

    void think() override
    {
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
                base.evaluation = base.board.adv_eval();
            }
            return;
        };

        // Create tree structure
        BoardTree root(board);

        if (root.board.get_turn() == Color::White)
            alphaBetaMax(root, -100000, 100000, 5);
        else
            alphaBetaMin(root, -100000, 100000, 5);

        //Perform search looking at capture nodes.
        //quiesce(root, -100000, 100000);

        minimax(root);

        bestmove = root.bestmove;
        evaluation = root.evaluation*turn;

        // End of function
        thinking = false;
    }
};

int main()
{
    ABPEngine engine;

    return 0;
}
