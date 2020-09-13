#include <iostream>
#include <random>

#include "Board.hpp"

int main()
{
    Board board;

    board.print();

    std::random_device r;
    std::mt19937 eng(r());

    for (std::uint8_t i = 0; i < 50; i++)
    {
        std::vector<Move> moves = board.getMoves();

        for (const Move &m : moves)
            std::cout << m.longform() << std::endl;

        std::uniform_int_distribution<std::uint8_t> dist(0, moves.size()-1);

        Move move = moves.at(dist(eng));

        std::cout << "Choosing move " << move.longform() << std::endl;

        board.performMove(move);

        board.print();
    }

    return 0;
}
