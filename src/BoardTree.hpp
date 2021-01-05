#ifndef BOARDTREE_HPP
#define BOARDTREE_HPP

#include "Board.hpp"

class BoardTree
{
public:
    BoardTree(const Board &parent_board)
        : board(parent_board)
    {
    }

    BoardTree(const Board &parent_board, const Move &parent_move, BoardTree *_parent_tree = nullptr)
        : board(parent_board), move(parent_move), parent_tree(_parent_tree)
    {
        board.perform_move(move);
    }

    void expand(MoveList& movelist, std::uint8_t n = 1)
    {
        std::vector<std::uint64_t> z_list;

        expand(movelist, z_list, n);
    }

    void expand(MoveList& movelist, std::vector<std::uint64_t> &z_list, std::uint8_t n)
    {
        board.get_moves(movelist, z_list);

        is_checkmate = movelist.is_checkmate;
        is_stalemate = movelist.is_stalemate;

        if (!expanded)
        {
            nodes.clear();
            nodes.reserve(movelist.size());

            for (const Move &m : movelist)
            {
                nodes.emplace_back(board, m, this);
            }

            expanded = true;
        }

        if (n != 1)
        {
            for (BoardTree &t : nodes)
            {
                std::uint64_t zob = t.board.get_zobrist();
                z_list.push_back(zob);
                t.expand(movelist, z_list, n-1);
                z_list.pop_back();
            }
        }
    }

    std::uint64_t depth(MoveList& movelist, std::uint8_t d)
    {
        if (d == 0)
            return 1;

        expand(movelist);

        std::uint64_t n = 0;
        for (BoardTree &b : nodes)
            n += b.depth(movelist, d-1);

        nodes.clear();
        expanded = false;

        return n;
    }

    double evaluation = 9999999;
    Move bestmove = Move(Square{63},Square{63}, MoveSpecial::Promotion, Piece::Queen);

    // Info
    bool is_checkmate = false;
    bool is_stalemate = false;

    // Used for monte carlo
    int visitcount = 0;
    double winscore = 0;

    // Used for score pruning
    double score = 0;

    std::vector<BoardTree> nodes;
    bool expanded = false;

    Board board;
    Move move;

    // Parent node
    BoardTree* parent_tree = nullptr;
};

#endif
