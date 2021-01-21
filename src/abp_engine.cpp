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

    double alphaBetaMax(BoardTree& base, double alpha, double beta, int depthleft, std::vector<std::uint64_t> &zob_list)
    {
        base.board.get_moves(movelist);

        if (depthleft == 0)
        {
            return base.board.adv_eval(movelist);
           // return quiesce(base, alpha, beta);
        }

        base.expand(movelist, zob_list, 1);

        if(base.nodes.size() == 0)
            return base.board.adv_eval(movelist);

        for (BoardTree &node : base.nodes)
        {
            std::uint64_t zob = node.board.get_zobrist();
            zob_list.push_back(zob);
            double score = alphaBetaMin(node, alpha, beta, depthleft - 1, zob_list);
            zob_list.pop_back();

            if(score >= beta)
                return beta;   // fail hard beta-cutoff
            if(score > alpha)
                alpha = score; // alpha acts like max in MiniMax
        }
        return alpha;
    }

    double alphaBetaMin(BoardTree& base, double alpha, double beta, int depthleft, std::vector<std::uint64_t> &zob_list)
    {
        base.board.get_moves(movelist);

        if (depthleft == 0)
        {
            return base.board.adv_eval(movelist);
            //return quiesce(base, alpha, beta);
        }

        base.expand(movelist, zob_list, 1);

        if(base.nodes.size() == 0)
            return base.board.adv_eval(movelist);

        for (BoardTree &node : base.nodes)
        {
            std::uint64_t zob = node.board.get_zobrist();
            zob_list.push_back(zob);
            double score = alphaBetaMax(node, alpha, beta, depthleft - 1, zob_list);
            zob_list.pop_back();

            if(score <= alpha)
                return alpha; // fail hard alpha-cutoff
            if(score < beta)
                beta = score; // beta acts like min in MiniMax
        }
        return beta;
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

        std::chrono::duration<double, std::milli> previous_ply(0);
        std::chrono::duration<double, std::milli> last_ply(0);
        int ply = 1;

        std::uint64_t time_left = w_time;
        std::uint64_t time_inc = w_inc;
        if (board.get_turn() == Color::Black)
        {
            time_left = b_time;
            time_inc = b_inc;
        }

        std::uint64_t max_time = std::min(time_inc + time_left/10, std::uint64_t{30000});
        std::uint64_t exp_time = 0;

        while ((max_time - time_spent > exp_time) && (ply <= 5))
        {
            auto tp = std::chrono::high_resolution_clock::now();

            if (root.board.get_turn() == Color::White)
                alphaBetaMax(root, -100000, 100000, ply, z_list);
            else
                alphaBetaMin(root, -100000, 100000, ply, z_list);

            //Perform search looking at capture nodes.
            //quiesce(root, 10000, -10000);

            minimax(root);

            std::chrono::duration<double> dur = std::chrono::high_resolution_clock::now() - tp;

            bestmove = root.bestmove;
            evaluation = root.evaluation*turn;

            ply++;
            previous_ply = last_ply;
            last_ply = dur;

            exp_time = std::min(static_cast<double>(last_ply.count()/previous_ply.count()), double{30})*last_ply.count();
        }

        // End of function
        thinking = false;
    }
};

int main()
{
    ABPEngine engine;

    return 0;
}
