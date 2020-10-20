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

    BoardTree(const Board &parent_board, const Move &parent_move)
        : board(parent_board), move(parent_move)
    {
        board.perform_move(move);
    }

    void expand(MoveList& movelist, std::uint8_t n = 1)
    {
        board.get_moves(movelist);

        if (!expanded)
        {
            nodes.clear();
            nodes.reserve(movelist.size());

            for (const Move &m : movelist)
            {
                nodes.emplace_back(board, m);
            }

            expanded = true;
        }

        if (n != 1)
        {
            for (BoardTree &t : nodes)
            {
                t.expand(movelist, n-1);
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

    double evaluation;
    Move bestmove;

    // Used for score pruning
    double score = 0;

    std::vector<BoardTree> nodes;
    bool expanded = false;

    Board board;
    Move move;
};

#endif
