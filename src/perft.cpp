#include <fstream>
#include <iostream>
#include <iterator>
#include <random>
#include <string>
#include <sstream>

#include "Board.hpp"

int main(int argc, char** argv)
{
    if (argc == 2 && std::string(argv[1]) == "suite")
    {
        std::ifstream testfile("../hartmann.epd");
        std::string delim = "; ";

        std::string line;
        while (std::getline(testfile, line))
        {
            size_t pos = 0;
            std::string token;
            std::string epd = "";
            bool epd_found = false;

            while (pos != std::string::npos)
            {
                pos = line.find(delim);
                if (pos != std::string::npos)
                {
                    token = line.substr(0, pos);
                    line.erase(0, pos + delim.length());
                }
                else
                {
                    token = line;
                }

                if (!epd_found)
                {
                    epd = token;
                    epd_found = true;
                    std::cout << epd << std::endl;
                }
                else
                {
                    int d = token.at(1)-'0';
                    std::uint64_t target = std::stoi(token.substr(3, token.size()));


                    Board base(epd);
                    //base.print();

                    BoardTree tree(base);

                    std::uint64_t result = tree.depth(d);

                    if (result == target)
                    {
                        std::cout << "D" << d << " hit. ";
                    }
                    else
                    {
                        std::cout << "D" << d << " miss. ";
                    }
                    std::cout << std::flush;
                }
            }

            std::cout << '\n' << std::endl;
        }

        return 0;
    }

    int goal = 6;
    std::string pos = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq -";

    if (argc > 1)
        goal = std::atoi(argv[1]);

    if (argc > 2)
    {
        if (argv[2] == std::string("kiwipete"))
            pos = "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq -";
        else if (argv[2] == std::string("pos3"))
            pos = "8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - -";
        else if (argv[2] == std::string("pos4"))
            pos = "r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq -";
        else if (argv[2] == std::string("pos5"))
            pos = "rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ -";
        else if (argv[2] == std::string("pos6"))
            pos = "r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - -";
    }

    Board base(pos);
    base.print();

    for (int i = 1; i <= goal; i++)
    {
        BoardTree tree(base);

        std::cout << "Perft " << i << " = " << tree.depth(i) << std::endl;
    }

    return 0;
}
