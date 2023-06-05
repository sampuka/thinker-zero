#include "Engine.hpp"

#include "console/uci_output.hpp"
#include "movegen/movegen.hpp"
#include "position/PositionAnalysis.hpp"

Engine::Engine() : rng(rd())
{

}

void Engine::main_loop()
{
    while (engine_running)
    {
        ;
    }
}

void Engine::shutdown()
{
    engine_running = false;
}

void Engine::new_game()
{
}

void Engine::go()
{
    MoveList pseudolegal_moves = generate_moves(position);

    // Filter illegal moves by trying and seeing if king is in check
    MoveList legal_moves;
    for (const Move& move : pseudolegal_moves)
    {
        Position new_pos = position;
        new_pos.make_move(move);

        PositionAnalysis analysis(new_pos);

        if (!analysis.king_in_check())
        {
            legal_moves.push_back(move);
        }
    }

    if (legal_moves.size() == 0)
    {
        uci_bestmove(Move("0000"));
    }
    else
    {
        std::uniform_int_distribution<uint8_t> uid(0, legal_moves.size()-1);

        uci_bestmove(legal_moves.at(uid(rng)));
    }
}

void Engine::set_position(Position new_position)
{
    position = new_position;
}

Position& Engine::get_position()
{
    return position;
}

void Engine::perform_move(const Move& move)
{
    position.make_move(move);
}