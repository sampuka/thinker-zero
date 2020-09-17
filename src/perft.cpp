#include <iostream>
#include <random>

#include "Board.hpp"

int main()
{
    Board base;

    for (int i = 1; i <= 5; i++)
    {
        BoardTree tree(base);

        std::cout << "Perft " << i << " = " << tree.depth(i) << std::endl;
    }

    return 0;
}
