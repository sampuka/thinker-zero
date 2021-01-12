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

    MoveList movelist;

    double alphaBetaMax(BoardTree& base, double alpha, double beta, int depthleft)
    {
        base.board.get_moves(movelist);

        if (depthleft == 0)
        {
            return base.board.adv_eval(movelist);
           // return quiesce(base, alpha, beta);
        }

        base.expand(movelist, 1);

        if(base.nodes.size() == 0)
            return base.board.adv_eval(movelist);

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
        base.board.get_moves(movelist);

        if (depthleft == 0)
        {
            return base.board.adv_eval(movelist);
            //return quiesce(base, alpha, beta);
        }

        base.expand(movelist, 1);

        if(base.nodes.size() == 0)
            return base.board.adv_eval(movelist);

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

    //Look at nodes which involves captures.
    double quiesce(BoardTree& base, double alpha, double beta)
    {
        base.board.get_moves(movelist);

        double stand_pat = base.board.adv_eval(movelist);
        if(stand_pat >= beta)
            return beta;
        if(alpha < stand_pat)
            alpha = stand_pat;

        if(base.board.typetohere == MoveType::Capture)
        {
            base.expand(movelist, 1);
            double score = stand_pat;
            for (BoardTree &node : base.nodes)
            {
                score = -quiesce(node, -beta, -alpha);
            }

            if(score >= beta)
                return beta;
            if(score > alpha)
               alpha = score;
        }
        return alpha;
    }

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

        if (root.board.get_turn() == Color::White)
            alphaBetaMax(root, -100000, 100000, 4);
        else
            alphaBetaMin(root, -100000, 100000, 4);

        //Perform search looking at capture nodes.
        //quiesce(root, 10000, -10000);

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
