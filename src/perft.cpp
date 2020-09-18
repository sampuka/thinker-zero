#include <iostream>
#include <random>

#include "Board.hpp"

int main(int argc, char** argv)
{
    int goal = 6;

    if (argc == 2)
        goal = std::atoi(argv[1]);

    // Kiwipete
    Board base("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq -");

    // Standard
    //Board base;

    for (int i = 1; i <= goal; i++)
    {
        BoardTree tree(base);

        std::cout << "Perft " << i << " = " << tree.depth(i) << std::endl;
    }

    return 0;
}
