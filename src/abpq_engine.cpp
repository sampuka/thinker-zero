#include "UCIEngine.hpp"

class ABPQEngine : public UCIEngine
{
public:
    ABPQEngine()
    {
        engine_name = "ABPQ Engine";
        engine_author = "Mathias Lyngbye and Martin Jakobsgaard";

        start();
    }

    MoveList movelist;

    double alphaBetaMax(BoardTree& base, double alpha, double beta, int depthleft)
    {
        base.board.get_moves(movelist);

        if (depthleft == 0)
        {
            //return base.board.adv_eval(movelist);
            return quiesce(base, alpha, beta);
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
            //return base.board.adv_eval(movelist);
            return quiesce(base, alpha, beta);
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
            return stand_pat; // some say that returning beta will make it kill itself --> https://stackoverflow.com/questions/48846642/is-there-something-wrong-with-my-quiescence-search
        if(alpha < stand_pat)
            alpha = stand_pat;
///////////////////////////////////////// DELTA PRUUNIN ////////////////////
        // get a "stand pat" score

        // double stand_pat = base.board.adv_eval(movelist);

        // check if it causes a beta cutoff

        if( stand_pat >= beta )
           return beta;

        // The next three lines test if alpha can be improved by greatest
        // possible material swing.

        int BIG_DELTA = 9; // queen value
        if ( base.board.movetohere.get_type()==MoveSpecial::Promotion ) BIG_DELTA += 7;

        if ( stand_pat < alpha - BIG_DELTA ) {
           return alpha;
        }

        if( alpha < stand_pat )
           alpha = stand_pat;


///////////////////////////////////////////////// END ///////////////////////
//One more hint: usually, Quiscence search uses a separate move generator that generates only captures,
//checks and pawn promotions (such a generator is simpler and faster than the normal one).


        if(base.board.typetohere == MoveType::Capture)
        {
            base.expand(movelist, 1);
            double score = stand_pat;
            for (BoardTree &node : base.nodes)
            {
                score = -quiesce(node, -beta, -alpha);
            }
                // undo last move --> restore state is this possible, i dont think it works without.
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
            alphaBetaMax(root, -100000, 100000, 3);
        else
            alphaBetaMin(root, -100000, 100000, 3);

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
    ABPQEngine engine;

    return 0;
}
