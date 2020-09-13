#include <iostream>

#include "Board.hpp"

int main()
{
    Board board;

    board.print();

    std::vector<Move> moves = board.getMoves();

    for (const Move &m : moves)
        std::cout << m.longform() << std::endl;

    return 0;
}
