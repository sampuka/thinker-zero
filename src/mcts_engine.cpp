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
        uint64_t time_max = 5000;
        while((time_spent < time_max) && !emergency_brake)
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
        std::cout << "Available moves:" << std::endl;
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

        // Print principal variation
        principal_variation(&root_node);


        // End of function
        thinking = false;
    }
    
    void principal_variation(BoardTree* root_node)
    {
        std::cout << "Principal variation:" << std::endl;
        BoardTree* eval_node = root_node;
        while(eval_node->nodes.size() > 0)
        {
            // BIG PRINT
            /*
            for(size_t i = 0; i < eval_node->nodes.size(); i++)
            {
                std::cout << "[Node #" << i << "]: Move: " << eval_node->nodes[i].move.longform() << " w:" << eval_node->nodes[i].winscore << "/v:" << eval_node->nodes[i].visitcount << "/wr:";
                double winrate = eval_node->nodes[i].winscore / eval_node->nodes[i].visitcount;
                std::cout << winrate;
                std::cout << "/uct:" << uct(eval_node->visitcount, eval_node->nodes[i].visitcount, eval_node->nodes[i].winscore) << std::endl;
            }
            */
            // END OF BIG PRINT
            BoardTree* best_child = nullptr;
            double bwr = -1;
            if(eval_node->board.get_turn() == Color::White)
            {
                bwr = 0;
                for(size_t i = 0; i < eval_node->nodes.size(); i++)
                {
                    double wr = eval_node->nodes[i].winscore / eval_node->nodes[i].visitcount;
                    if(wr > bwr)
                    {
                        bwr = wr;
                        best_child = &(eval_node->nodes[i]);
                    }
                }
            }
            else
            {
                bwr = 999999;
                for(size_t i = 0; i < eval_node->nodes.size(); i++)
                {
                    double wr = eval_node->nodes[i].winscore / eval_node->nodes[i].visitcount;
                    if(wr < bwr)
                    {
                        bwr = wr;
                        best_child = &(eval_node->nodes[i]);
                    }
                }

            }
            eval_node = best_child;
            std::cout << "Picked [" << eval_node->move.longform() << "], wr:" << bwr << ", visit:" << eval_node->visitcount << ", wins:" << eval_node->winscore <<  std::endl;
        }
    }
    
    // Input:  Tree node
    // Output: Normalized unbiased end state evaluation (1,0,-1)
    int simulate_playout_random(BoardTree *root_node)
    {
        // Generate possible moves
        MoveList tmp_moves;
        Board tmp_board = root_node->board;
        tmp_board.get_moves(tmp_moves);

        // Check if state is win
        if(tmp_moves.is_checkmate)
        {
            // Get board turn
            int board_turn = 0;
            if(tmp_board.get_turn() == Color::White)
                board_turn = 1;
            else
                board_turn = -1;
            board_turn *= -turn_bias; // Negative because you are in check mate

            return board_turn;
        }

        // Check if state is stale
        if(tmp_moves.is_stalemate)
            return 0;
       
        // Save start eval
        double eval_start = tmp_board.adv_eval(tmp_moves);

        // Find end state (win/lose/draw/max iterations)
        int max_moves = 15;
        int moves = 0;
        Move random_move;
        while( !tmp_moves.is_checkmate
            && !tmp_moves.is_stalemate
            && moves < max_moves )
        {
            // Generate random move
            std::uniform_int_distribution<std::uint8_t> dist(0, tmp_moves.size()-1);
            random_move = tmp_moves.at(dist(eng));
            
            // Perform move and get moves of next state
            tmp_board.perform_move(random_move);
            tmp_board.get_moves(tmp_moves);

            // Ensure max iterations
            moves++;
        }
        
        // Evaluate end state (this needs turn bias?)
        double eval = tmp_board.adv_eval(tmp_moves);    // Rough double to int conversion
        eval -= eval_start;                             // Optional: relative evaluation
        
        // Translate evaluation
        double padding = 0.5;
        int eval_norm = 0;
        if (eval > padding)
            eval_norm = 1;
        if (eval < -padding)
            eval_norm = -1;
        eval_norm *= turn_bias;

        return eval_norm;
    }

    void backpropagation(BoardTree* node, int playout_result)
    {
        BoardTree* eval_node = node;
        while(eval_node != nullptr)
        {
            // Increment visit count regardless
            eval_node->visitcount++;

            // Get board turn
            int board_turn = 0;
            if(eval_node->board.get_turn() == Color::White)
                board_turn = 1;
            else
                board_turn = -1;
            board_turn *= -turn_bias; // Negated to consider turn-to-move

            // Increment win count if win
            if(board_turn == playout_result)
                eval_node->winscore++;
            
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
        //c = 0.35;

        // Avoid divide by zero
        if(node_visit == 0)
        {
            return 9999999;
        }

        return ((node_score / (double)node_visit) + c * std::sqrt(std::log(parent_visit) / (double)node_visit));
    }

    // Member variables
    int turn_bias = 0;  // Needed for evals
    bool emergency_brake = false;
};

int main()
{
    MctsEngine engine;

    return 0;
}
