#include <iostream>
#include <random>

#include "Board.hpp"

int main(int argc, char** argv)
{
    int goal = 6;

    if (argc == 2)
        goal = std::atoi(argv[1]);

    Board base;

    for (int i = 1; i <= goal; i++)
    {
        BoardTree tree(base);

        std::cout << "Perft " << i << " = " << tree.depth(i) << std::endl;
    }

    return 0;
}
