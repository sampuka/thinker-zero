#ifndef UCI_ENGINE_HPP
#define UCI_ENGINE_HPP

#include "Board.hpp"
#include "BoardTree.hpp"

#include <algorithm>
#include <atomic>
#include <chrono>
#include <fstream>
#include <iostream>
#include <iterator>
#include <mutex>
#include <random>
#include <sstream>
#include <string>
#include <thread>
#include <vector>

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

    std::string engine_name = "Unspecified engine";
    std::string engine_author = "Unspecified author";

    // Current player times and increment [ms]
    std::atomic<std::uint64_t> time_spent = 0; // Time passed since started thinking
    std::atomic<std::uint64_t> w_time = 0;
    std::atomic<std::uint64_t> b_time = 0;
    std::atomic<std::uint64_t> w_inc = 0;
    std::atomic<std::uint64_t> b_inc = 0;
    std::atomic<bool> infinite = false;

    Move bestmove;
    std::atomic<double> evaluation = 0;
    std::atomic<bool> thinking = false;

    std::vector<std::uint64_t> z_list;

    std::mt19937 eng;

    std::ofstream log;

    void start()
    {
        rx_thread = std::thread(&UCIEngine::rx_loop, this);
        state_loop();
    }

private:
    void rx_loop()
    {
        while (1)
        {
            std::string cmd;
            std::getline(std::cin, cmd);

            if (cmd == "")
                continue;

            log << "> " << cmd << std::endl;

            std::vector<std::string> tokens;

            std::istringstream iss(cmd);
            std::copy(std::istream_iterator<std::string>(iss),
                    std::istream_iterator<std::string>(),
                    std::back_inserter(tokens));

            rx_buffer_mutex.lock();
            rx_buffer.push_back(tokens);
            rx_buffer_mutex.unlock();
        }
    }

    void state_loop()
    {
        while (1)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));

            time_spent = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - start_thinking_ts).count();

            while (rx_buffer.size() != 0)
            {
                rx_buffer_mutex.lock();
                std::vector<std::string> tokens = rx_buffer.at(0);
                rx_buffer.erase(rx_buffer.begin());
                rx_buffer_mutex.unlock();

                if (tokens.at(0) == "uci")
                {
                    send_cmd("id name " + engine_name);
                    send_cmd("id author " + engine_author);
                    send_cmd("uciok");
                }

                else if (tokens.at(0) == "isready")
                {
                    send_cmd("readyok");
                }

                else if (tokens.at(0) == "quit")
                {
                    return;
                }

                else if (tokens.at(0) == "position")
                {
                    z_list.clear();
                    std::uint32_t i = 1;

                    if (tokens.at(1) == "startpos")
                    {
                        board = Board();
                        i++;
                    }
                    else if (tokens.at(1) == "fen")
                    {
                        std::string fen_string =
                            tokens.at(2) + ' ' +
                            tokens.at(3) + ' ' +
                            tokens.at(4) + ' ' +
                            tokens.at(5) + ' ' +
                            tokens.at(6) + ' ' +
                            tokens.at(7);

                        board = Board(fen_string);
                        i+=7;
                    }

                    z_list.push_back(board.get_zobrist());

                    if (i != tokens.size() && tokens.at(i) == "moves")
                    {
                        while (++i < tokens.size())
                        {
                            Move move_(tokens.at(i), Color::Empty, Piece::None, 9); // Used for get_from()
                            Piece p = board.get_piece(move_.get_from());

                            Move move(tokens.at(i),
                                    board.get_turn(),
                                    p,
                                    board.get_ep());

                            board.perform_move(move);

                            if (
                                    (p == Piece::Pawn) ||
                                    (board.typetohere == MoveType::Capture) ||
                                    (move.get_type() != MoveSpecial::None)
                               )
                            {
                                z_list.clear();
                            }

                            z_list.push_back(board.get_zobrist());
                        }
                    }

                    board.print(log);
                    for (const std::uint64_t z : z_list)
                        log << z << ' ';
                    log << std::endl;
                }

                else if (tokens.at(0) == "go")
                {
                    start_thinking_ts = std::chrono::steady_clock::now();
                    time_spent = 0;

                    if (tokens.size() > 1 && tokens.at(1) == "perft")
                    {
                        board.print();
                        std::uint8_t d = std::stoi(tokens.at(2)) - 1;
                        std::uint64_t total = 0;
                        MoveList startlist;
                        board.get_moves(startlist);
                        std::vector<BoardTree> trees;
                        trees.reserve(startlist.size());

                        MoveList movelist;
                        for (std::uint16_t i = 0; i < startlist.size(); i++)
                        {
                            const Move &m = startlist.at(i);
                            trees.emplace_back(board, m);

                            std::uint64_t n = trees.at(i).depth(movelist, d);
                            total += n;

                            std::cout << m.longform() << ": " << n << '\n';
                        }

                        std::cout << "Total: " << total << std::endl;
                    }
                    else
                    {
                        std::uint8_t i = 0;
                        while (++i < tokens.size())
                        {
                            if (tokens.at(i) == "wtime")
                                w_time = std::stoi(tokens.at(++i));

                            if (tokens.at(i) == "btime")
                                b_time = std::stoi(tokens.at(++i));

                            if (tokens.at(i) == "winc")
                                w_inc = std::stoi(tokens.at(++i));

                            if (tokens.at(i) == "binc")
                                b_inc = std::stoi(tokens.at(++i));

                            if (tokens.at(i) == "infinite")
                            {
                                i++;
                                infinite = true;
                            }
                        }

                        log << "wtime/btime/winc/binc = " << w_time << '/' << b_time << '/' << w_inc << '/' << b_inc << std::endl;

                        thinking = true;
                        think_state = true;
                        think_thread = std::thread(&UCIEngine::think, this);
                    }
                }
                else if (tokens.at(0) == "puzzle")
                {
                    if (tokens.at(1) == "1")
                    {
                        board = Board("8/8/8/qn6/kn6/1n6/1KP5/8 w - - 0 1");
                        std::cout << "Solution c2b3" << std::endl;
                    }
                    else if (tokens.at(1) == "2")
                    {
                        board = Board("1Bb3BN/R2Pk2r/1Q5B/4q2R/2bN4/4Q1BK/1p6/1bq1R1rb w - - 0 1");
                        std::cout << "Solution e3a3" << std::endl;
                    }
                    else if (tokens.at(1) == "3")
                    {
                        board = Board("Q2bRBNN/K1kqRpp1/p1Prp3/p1pp4/6P1/5PP1/2P1Pp2/7n w - - 0 1");
                        std::cout << "Solution e3a3" << std::endl;
                    }

                    board.print();

                    start_thinking_ts = std::chrono::steady_clock::now();
                    time_spent = 0;
                    w_time = 10000;
                    b_time = 10000;
                    w_inc = 1000;
                    b_inc = 1000;
                    thinking = true;
                    think_state = true;
                    think_thread = std::thread(&UCIEngine::think, this);
                }
            }

            // State machine ish
            if (think_state)
            {
                if (!thinking)
                {
                    think_thread.join();
                    think_state = false;

                    send_cmd("bestmove " + bestmove.longform());
                    send_cmd("info score cp " + std::to_string(static_cast<int>(evaluation*100)));
                }
            }
        }
    }

    void send_cmd(std::string s)
    {
        log << "< " << s << std::endl;
        std::cout << s << std::endl;
    }

    std::chrono::steady_clock::time_point start_thinking_ts;

    std::vector<std::vector<std::string>> rx_buffer;
    std::mutex rx_buffer_mutex;

    std::thread rx_thread;
    std::thread think_thread;
    bool think_state = false;

    std::random_device r;
};

#endif
