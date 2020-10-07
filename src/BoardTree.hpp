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

    void expand(std::uint8_t n = 1)
    {
        std::vector<Move> &moves = board.get_moves();

        if (!expanded)
        {
            nodes.reserve(moves.size());

            for (const Move &m : moves)
            {
                nodes.emplace_back(board, m);
            }

            expanded = true;
        }

        if (n != 1)
        {
            for (BoardTree &t : nodes)
            {
                t.expand(n-1);
            }
        }
    }

    std::uint64_t depth(std::uint8_t d)
    {
        if (d == 0)
            return 1;

        expand();

        std::uint64_t n = 0;
        for (BoardTree &b : nodes)
            n += b.depth(d-1);

        nodes.clear();
        expanded = false;

        return n;
    }

    double evaluation;
    Move bestmove = Move(0, 0, 0, 0);

    std::vector<BoardTree> nodes;
    bool expanded = false;

    Board board;
    Move move = Move(0, 0, 0, 0);
};

#endif
