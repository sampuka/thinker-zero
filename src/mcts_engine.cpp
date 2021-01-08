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
        // Set turn bias
        turn_bias = 1;
        if(board.get_turn() == Color::Black)
            turn_bias = -1;

        // Initialization
        MoveList startmoves;
        board.get_moves(startmoves);
        bestmove = startmoves.at(1);

        // Root node
        BoardTree root_node(board);

        // MCTS
        uint64_t time_max = 1000;
        while(time_spent < time_max)
        {
            // === Selection ===
            BoardTree* promising_node = select_node(&root_node);

            // === Expansion ===
            promising_node->expand(movelist);
            BoardTree* node_to_explore = promising_node;
            
            // If expansion yielded children, pick a random child
            if(promising_node->nodes.size() > 0)
            {
                std::uniform_int_distribution<std::uint8_t> dist(0, promising_node->nodes.size()-1);
                node_to_explore = &promising_node->nodes.at(dist(eng));
            }

            // === Simulation ===
            int playout_result = simulate_playout_random(node_to_explore);

            // === Backpropagation ===
            backpropagation(node_to_explore, playout_result);
        }
        
        // Find best move (new)
        double bestwinrate = 0;
        for(size_t i = 0; i < root_node.nodes.size(); i++)
        {
            std::cout << "[Node #" << i << "]: Move: " << root_node.nodes[i].move.longform() << " w:" << root_node.nodes[i].winscore << "/v:" << root_node.nodes[i].visitcount << "/wr:";
            double winrate = root_node.nodes[i].winscore / root_node.nodes[i].visitcount;
            std::cout << winrate;
            std::cout << "/uct:" << uct(root_node.visitcount, root_node.nodes[i].visitcount, root_node.nodes[i].winscore) << std::endl;
            if(winrate > bestwinrate)
            {
                bestwinrate = winrate;
                bestmove = root_node.nodes[i].move;
            }
        }
        
        // Find best move (old)
        //bestmove = find_best_child(&root_node)->move;

        // End of function
        thinking = false;
    }

    // Input:  Tree node
    // Output: Normalized unbiased end state evaluation (1,0,-1)
    int simulate_playout_random(BoardTree *root_node)
    {
        // Generate possible moves
        MoveList tmp_moves;
        Board tmp_board = root_node->board;
        tmp_board.get_moves(tmp_moves);
        
        // Find end state (win/lose/draw/max iterations)
        int max_moves = 50;
        int moves = 0;
        while( !tmp_moves.is_checkmate
            && !tmp_moves.is_stalemate
            && moves < max_moves )
        {
            // Generate random move
            std::uniform_int_distribution<std::uint8_t> dist(0, tmp_moves.size()-1);
            Move random_move = tmp_moves.at(dist(eng));
            
            // Perform move and get moves of next state
            tmp_board.perform_move(random_move);
            tmp_board.get_moves(tmp_moves);
            
            // Ensure max iterations
            moves++;
        }

        // Output evaluation at end
        std::cout << "============================" << std::endl;
        std::cout << "[Eval] " << tmp_board.basic_eval(tmp_moves) << std::endl;
        tmp_board.print();
        
        // Evaluate end state (this needs turn bias?)
        int eval = tmp_board.basic_eval(tmp_moves);             // Rough double to int conversion

        int eval_norm = 0;
        if(eval != 0)
            eval_norm = (eval/std::abs(eval)) * turn_bias;      // Normalized evaluation (1,0,-1)

        return eval_norm;
    }

    void backpropagation(BoardTree* node, int playout_result)
    {
        BoardTree* eval_node = node;
        std::cout << "Backpropagation:" << std::endl;
        std::cout << "Playout result: " << playout_result << std::endl;
        while(eval_node != nullptr)
        {
            // Output before propagation
            std::cout << "[turn: " << (eval_node->board.get_turn() == Color::White ? "White" : "Black") << "] ";
            std::cout << "w:" << eval_node->winscore << '/';
            std::cout << "v:" << eval_node->visitcount << " -> ";

            // Increment visit count regardless
            eval_node->visitcount++;

            // Get board turn
            int board_turn = 0;
            if(eval_node->board.get_turn() == Color::White)
                board_turn = 1;
            else
                board_turn = -1;
            board_turn *= turn_bias; 

            // Increment win count if win
            if(board_turn == playout_result)
                eval_node->winscore++;
            
            // Output after propagation
            std::cout << "w:" << eval_node->winscore << '/';
            std::cout << "v:" << eval_node->visitcount << std::endl;

            // Set current node -> parent node
            eval_node = eval_node->parent_tree;
        }
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
            double node_a_uct = uct(parent_visitcount, a.visitcount, a.winscore);
            double node_b_uct = uct(parent_visitcount, b.visitcount, b.winscore);
            return (node_a_uct < node_b_uct);
        };

        return &(*std::max_element(node->nodes.begin(), node->nodes.end(), comparator));
    }

    double uct(int parent_visit, int node_visit, double node_score)
    {
        double c = std::sqrt(2.0); // Theoretical value

        // Avoid divide by zero
        if(node_visit == 0)
        {
            return 9999999;
        }

        return ((node_score / (double)node_visit) + c * std::sqrt(std::log(parent_visit) / (double)node_visit));
    }

    // Member variables
    int turn_bias = 0;  // Needed for evals
    //int win_score = 10;
};

int main()
{
    MctsEngine engine;

    return 0;
}
