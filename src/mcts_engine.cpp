#include "UCIEngine.hpp"

#include <functional>
#include <cmath>

class MctsEngine : public UCIEngine
{
public:
    MctsEngine()
    {
        engine_author = "Mathias Lyngbye";
        engine_name = "MCTS Engine";
        start();
    }

    MoveList movelist;

    void think() override
    {
        // Default white
        /*
        int turn_bias = 1;
        if(board.get_turn() == Color::Black)
            turn_bias = -1;
        */

        // Initialization
        MoveList startmoves;
        board.get_moves(startmoves);
        bestmove = startmoves.at(1);

        // Root node
        BoardTree root_node(board);

        // MCTS
        uint64_t time_max = 2000;
        while(time_spent < time_max)
        {
            // Selection
            BoardTree *promising_node = select_node(&root_node);

            // Expansion
            promising_node->expand(movelist);
        }

        // End of function
        thinking = false;
    }

    int simulate_playout_random(BoardTree *root_node)
    {
        MoveList tmp_moves;
        
        Board tmp_board = root_node->board;
        tmp_board.get_moves(tmp_moves);

        while(!tmp_board.is_checkmate(tmp_moves) && !tmp_board.is_stalemate(tmp_moves))
        {
            // Generate random move
            std::uniform_int_distribution<std::uint8_t> dist(0, tmp_moves.size()-1);
            Move random_move = tmp_moves.at(dist(eng));
            
            tmp_board.perform_move(random_move);
            tmp_board.get_moves(tmp_moves);
        }

        // NOTE: This function is not finished.
        return 0;
    }

    BoardTree *select_node(BoardTree *root_node)
    {
        BoardTree *node = root_node;
        while(node->nodes.size() != 0)  // While not a leaf
        {
            node = find_best_child(node);
        }
        
        return node;
    }

    BoardTree *find_best_child(BoardTree *node)
    {
        int parent_visitcount = node->visitcount;
        const auto comparator = [&](const BoardTree &a, const BoardTree &b)
        {
            double node_a_uct = uct(parent_visitcount, a.winscore, a.visitcount);
            double node_b_uct = uct(parent_visitcount, b.winscore, b.visitcount);
            return (node_a_uct < node_b_uct);
        };

        return &(*std::max_element(node->nodes.begin(), node->nodes.end(), comparator));
    }

    double uct(int parent_visit, int node_visit, double node_score)
    {
        double c = std::sqrt(2.0); // Theoretical value
        return ((node_score / node_visit) + c * std::sqrt(std::log(parent_visit) / node_visit));
    }
};

int main()
{
    MctsEngine engine;

    return 0;
}
