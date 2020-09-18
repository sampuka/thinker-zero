#include <iostream>
#include <random>
#include <string>

#include "Board.hpp"

int main(int argc, char** argv)
{
    int goal = 6;

    if (argc > 1)
        goal = std::atoi(argv[1]);

    Board base;

    if (argc > 2)
    {
        if (argv[2] == std::string("kiwipete"))
            base = Board("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq -");
    }

    base.print();

    for (int i = 1; i <= goal; i++)
    {
        BoardTree tree(base);

        std::cout << "Perft " << i << " = " << tree.depth(i) << std::endl;
    }

    return 0;
}
