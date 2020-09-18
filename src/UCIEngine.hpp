#ifndef UCI_ENGINE_HPP
#define UCI_ENGINE_HPP

#include "Board.hpp"

#include <algorithm>
#include <atomic>
#include <fstream>
#include <iostream>
#include <iterator>
#include <random>
#include <sstream>
#include <string>
#include <thread>

class UCIEngine
{
public:
    UCIEngine()
    {
        thinking = false;
        eng = std::mt19937(r());
        log.open("/tmp/UCI_Engine.log");
    }

    virtual void think() = 0;

protected:
    Board board;

    std::ofstream log;

    void start()
    {
        rx_loop();
    }

    void rx_loop()
    {
        while (1)
        {
            std::string cmd;
            std::getline(std::cin, cmd);

            log << "> " << cmd << std::endl;

            std::vector<std::string> tokens;

            std::istringstream iss(cmd);
            std::copy(std::istream_iterator<std::string>(iss),
                    std::istream_iterator<std::string>(),
                    std::back_inserter(tokens));

            if (tokens.at(0) == "uci")
            {
                send_cmd("id name Random Engine");
                send_cmd("id author Mathias Jensen");
                send_cmd("uciok");
            }

            else if (tokens.at(0) == "isready")
            {
                send_cmd("readyok");
            }

            else if (tokens.at(0) == "position")
            {
                if (tokens.at(1) == "startpos")
                {
                    board = Board();

                    for (std::uint32_t i = 3; i < tokens.size(); i++)
                        board.performMove(Move(tokens.at(i)));
                }
                else if (tokens.at(1) == "fen")
                {
                    board = Board(cmd.substr(13, cmd.size()-1));
                }

                board.print(log);
            }

            else if (tokens.at(0) == "go")
            {
                if (tokens.at(1) == "perft")
                {
                    board.print();
                    std::uint8_t d = std::stoi(tokens.at(2)) - 1;
                    std::uint64_t total = 0;
                    std::vector<Move> moves = board.getMoves();
                    std::vector<BoardTree> trees;
                    trees.reserve(moves.size());

                    for (std::uint16_t i = 0; i < moves.size(); i++)
                    {
                        const Move &m = moves.at(i);
                        trees.emplace_back(board, m);

                        std::uint64_t n = trees.at(i).depth(d);
                        total += n;

                        std::cout << m.longform() << ": " << n << '\n';
                    }

                    std::cout << "Total = " << total << std::endl;
                }
                else
                {
                    thinking = true;
                    think_thread = std::thread(&UCIEngine::think, this);

                    while (thinking)
                        ;
                    think_thread.join();

                    send_cmd("bestmove " + bestmove.longform());
                }
            }
        }
    }

    void send_cmd(std::string s)
    {
        log << "< " << s << std::endl;
        std::cout << s << std::endl;
    }

    std::thread think_thread;

    Move bestmove = Move(0, 0, 0, 0);
    std::atomic<bool> thinking;

    std::random_device r;
    std::mt19937 eng;
};

#endif
