#include "UCIEngine.hpp"

class ABPV2Engine : public UCIEngine
{
public:
    ABPV2Engine()
    {
        engine_name = "ABP v2 Engine";
        engine_author = "Mathias Ebbensgaard Jensen";

        start();
    }

    MoveList movelist;

    double alphaBetaMax(Board& base, double alpha, double beta, int depthleft, std::vector<std::uint64_t> &zob_list)
    {
        base.get_moves(movelist, zob_list);

        if (depthleft == 0)
        {
            return base.adv_eval(movelist);
        }

        if (movelist.size() == 0)
        {
            return base.adv_eval(movelist);
        }

        for (int i = 0; i < movelist.size(); i++)
        {
            Board node(base, movelist.at(i));

            std::uint64_t zob = node.get_zobrist();

            TValue tv = tt->read(zob);
            if (tv.zobrist_key == zob)
            {
                //std::cout << "TT hit for " << zob << std::endl;
            }
            else
            {
                if (tv.zobrist_key == 0)
                {
                    //std::cout << "TT new for " << zob << std::endl;
                }
                else
                {
                    std::cout << "TT overwrite for " << zob << std::endl;
                }
                tt->insert(zob);
            }

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

    double alphaBetaMin(Board& base, double alpha, double beta, int depthleft, std::vector<std::uint64_t> &zob_list)
    {
        base.get_moves(movelist);

        if (depthleft == 0)
        {
            return base.adv_eval(movelist);
        }

        if (movelist.size() == 0)
        {
            return base.adv_eval(movelist);
        }

        for (int i = 0; i < movelist.size(); i++)
        {
            Board node(base, movelist.at(i));

            std::uint64_t zob = node.get_zobrist();
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

        std::uint64_t max_time = std::min(time_inc + time_left/4, std::uint64_t{30000});
        std::uint64_t exp_time = 0;

        MoveList root_moves;
        board.get_moves(root_moves);
        std::vector<Board> root_boards;
        for (int i = 0; i < root_moves.size(); i++)
        {
            root_boards.emplace_back(board, root_moves.at(i));
        }

        while (max_time - time_spent > exp_time)
        {
            auto tp = std::chrono::high_resolution_clock::now();

            std::vector<double> evals(root_moves.size());
            for (int i = 0; i < root_moves.size(); i++)
            {
                if (board.get_turn() == Color::White)
                    evals.at(i) = alphaBetaMin(root_boards.at(i), -100000, 100000, ply, z_list);
                else
                    evals.at(i) = alphaBetaMax(root_boards.at(i), -100000, 100000, ply, z_list);
            }

            double best_move = 0;
            double best_eval = evals.at(0)*turn;

            for (int i = 1; i < root_moves.size(); i++)
            {
                if (evals.at(i)*turn > best_eval)
                {
                    best_move = i;
                    best_eval = evals.at(i)*turn;
                }
            }

            std::chrono::duration<double> dur = std::chrono::high_resolution_clock::now() - tp;

            bestmove = root_moves.at(best_move);
            evaluation = best_eval;

            if (evaluation > 150)
            {
                break;
            }

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
    ABPV2Engine engine;

    return 0;
}
