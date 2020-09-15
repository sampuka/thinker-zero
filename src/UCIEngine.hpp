#ifndef UCI_ENGINE_HPP
#define UCI_ENGINE_HPP

#include "Board.hpp"

#include <algorithm>
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
                    board = Board();
                else
                    log << "not startpos???" << std::endl;

                for (std::uint32_t i = 3; i < tokens.size(); i++)
                    board.performMove(Move(tokens.at(i)));

                board.print(log);

                std::vector<Move> moves = board.getMoves();

                for (const Move &move : moves)
                    log << move.longform() << std::endl;
            }

            else if (tokens.at(0) == "go")
            {
                thinking = true;
                think_thread = std::thread(&UCIEngine::think, this);

                while (thinking)
                    ;
                think_thread.join();

                thinking = false;
                send_cmd("bestmove " + bestmove.longform());
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
    bool thinking = false;

    std::random_device r;
    std::mt19937 eng;
};

#endif
