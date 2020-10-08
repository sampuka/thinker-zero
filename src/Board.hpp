#ifndef BOARD_HPP
#define BOARD_HPP

#include "utility.hpp"
#include "Bitboard.hpp"
#include "Move.hpp"
#include "movegen_rays.hpp"

#include <array>
#include <cstdint>
#include <iostream>
#include <iterator>
#include <limits>
#include <sstream>
#include <string>
#include <vector>

class Board
{
public:
    Board(const Board &b) :
        colors(b.colors),
        pieces(b.pieces),
        movelist_found(false),
        turn(b.turn),
        can_castle(b.can_castle),
        ep_x(b.ep_x)
    {}

    Board(std::string FEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1")
    {
        colors.at(static_cast<std::uint8_t>(Color::Empty)).board = ~0;

        std::vector<std::string> tokens;

        std::istringstream iss(FEN);
        std::copy(std::istream_iterator<std::string>(iss),
                std::istream_iterator<std::string>(),
                std::back_inserter(tokens));

        std::uint8_t x = 0;
        std::uint8_t y = 7;

        for (const char &c : tokens.at(0))
        {
            if (c > '0' && c <= '8')
            {
                x += c-'0';
                continue;
            }

            if (c == '/')
            {
                y--;
                x = 0;
                continue;
            }

            set_tile(x, y, char_to_tile(c));
            x++;
        }

        if (tokens.at(1).at(0) == 'w')
        {
            turn = Color::White;
        }
        else
        {
            turn = Color::Black;
        }

        can_castle.at(0).at(0) = false;
        can_castle.at(0).at(1) = false;
        can_castle.at(1).at(0) = false;
        can_castle.at(1).at(1) = false;

        for (char c : tokens.at(2))
        {
            if (c == 'K')
                can_castle.at(0).at(0) = true;
            if (c == 'Q')
                can_castle.at(0).at(1) = true;
            if (c == 'k')
                can_castle.at(1).at(0) = true;
            if (c == 'q')
                can_castle.at(1).at(1) = true;
        }

        // Ensure castling ability
        {
            Tile t = get_tile(7, 0);

            if (t.color != Color::White || t.piece != Piece::Rook)
                can_castle.at(0).at(0) = false;
        }
        {
            Tile t = get_tile(0, 0);

            if (t.color != Color::White || t.piece != Piece::Rook)
                can_castle.at(0).at(1) = false;
        }
        {
            Tile t = get_tile(7, 7);

            if (t.color != Color::Black || t.piece != Piece::Rook)
                can_castle.at(1).at(0) = false;
        }
        {
            Tile t = get_tile(0, 7);

            if (t.color != Color::Black || t.piece != Piece::Rook)
                can_castle.at(1).at(1) = false;
        }
        {
            Tile t = get_tile(4, 0);

            if (t.color != Color::White || t.piece != Piece::King)
            {
                can_castle.at(0).at(0) = false;
                can_castle.at(0).at(1) = false;
            }
        }
        {
            Tile t = get_tile(4, 7);

            if (t.color != Color::Black || t.piece != Piece::King)
            {
                can_castle.at(1).at(0) = false;
                can_castle.at(1).at(1) = false;
            }
        }

        if (tokens.at(3).at(0) == '-')
        {
            ep_x = 9;
        }
        else
        {
            ep_x = tokens.at(3).at(0) - 'a';
        }
    }

    Color get_color(std::uint8_t x, std::uint8_t y) const
    {
        for (std::uint8_t c = 0; c < 3; c++)
        {
            if (colors.at(c).read(x, y))
            {
                return static_cast<Color>(c);
            }
        }

        return Color::Empty;
    }

    Piece get_piece(std::uint8_t x, std::uint8_t y) const
    {
        for (std::uint8_t p = 0; p < 6; p++)
        {
            if (pieces.at(p).read(x, y))
            {
                return static_cast<Piece>(p);
            }
        }

        return Piece::None;
    }

    Tile get_tile(std::int8_t x_, std::int8_t y_) const
    {
        if (x_ < 0 || x_ > 7 || y_ < 0 || y_ > 7)
        {
            return Tile{Color::Empty, Piece::None, true};
        }

        std::uint8_t x = static_cast<std::uint8_t>(x_);
        std::uint8_t y = static_cast<std::uint8_t>(y_);

        return Tile{get_color(x, y), get_piece(x, y)};
    }

    void set_tile(std::int8_t x_, std::int8_t y_, Tile tile)
    {
        if (x_ < 0 || x_ > 7 || y_ < 0 || y_ > 7)
        {
            std::cerr << "Set tile out of bounds (" << std::to_string(x_) << ", " << std::to_string(y_) << ")" << std::endl;
        }

        std::uint8_t x = static_cast<std::uint8_t>(x_);
        std::uint8_t y = static_cast<std::uint8_t>(y_);

        for (std::uint8_t c = 0; c < 3; c++)
        {
            colors.at(c).write(x, y, false);
        }

        for (std::uint8_t p = 0; p < 6; p++)
        {
            pieces.at(p).write(x, y, false);
        }

        colors.at(static_cast<std::uint8_t>(tile.color)).write(x, y, true);

        if (tile.piece != Piece::None)
        {
            pieces.at(static_cast<std::uint8_t>(tile.piece)).write(x, y, true);
        }
    }

    Bitboard get_bitboard(Color color, Piece piece) const
    {
        Bitboard res;
        res.board =
                colors.at(static_cast<std::uint8_t>(color)).board &
                pieces.at(static_cast<std::uint8_t>(piece)).board;
        return res;
    }

    std::array<Bitboard, 64>& ray_movegen(bool only_threat = false) const
    {
        if (movelist_found)
            return ray_movelist;

        Bitboard all_blockers;
        all_blockers.board = ~colors.at(static_cast<std::uint8_t>(Color::Empty)).board;

        enemy_threat.board = 0;
        std::uint8_t king_index = 0;

        for (std::uint8_t x = 0; x < 8; x++)
        {
            for (std::uint8_t y = 0; y < 8; y++)
            {
                const Tile tile = get_tile(x, y);

                std::uint8_t index = y*8+x;

                Bitboard attacks;

                switch (tile.piece)
                {
                    case Piece::Pawn: // Only attacking moves
                        {
                            if (tile.color == turn)
                            {
                                Bitboard enemy_pieces;
                                if (turn == Color::White)
                                    enemy_pieces.board = colors[static_cast<std::uint8_t>(Color::Black)].board;
                                else
                                    enemy_pieces.board = colors[static_cast<std::uint8_t>(Color::White)].board;

                                if (ep_x != 9)
                                {
                                    if (turn == Color::White)
                                        enemy_pieces.write(ep_x, 5, true);
                                    else
                                        enemy_pieces.write(ep_x, 2, true);
                                }

                                if (turn == Color::White)
                                {
                                    attacks.board = movegen_rays[static_cast<std::uint8_t>(Ray::WhitePawnAttacks)][index].board & enemy_pieces.board;
                                }
                                else
                                {
                                    attacks.board = movegen_rays[static_cast<std::uint8_t>(Ray::BlackPawnAttacks)][index].board & enemy_pieces.board;
                                }
                            }
                            else
                            {
                                if (tile.color == Color::White)
                                {
                                    attacks.board = movegen_rays[static_cast<std::uint8_t>(Ray::WhitePawnAttacks)][index].board;
                                }
                                else
                                {
                                    attacks.board = movegen_rays[static_cast<std::uint8_t>(Ray::BlackPawnAttacks)][index].board;
                                }
                            }
                        }
                        break;

                    case Piece::Knight:
                        {
                            attacks.board = movegen_rays[static_cast<std::uint8_t>(Ray::Knight)][index].board;
                        }
                        break;

                    case Piece::Bishop:
                        {
                            for (std::uint8_t d = 0; d < 8; d+=2)
                            {
                                attacks.board |= movegen_rays[d][index].board;

                                Bitboard blockers;
                                blockers.board = movegen_rays[d][index].board & all_blockers.board;
                                if (blockers.board != 0)
                                {
                                    std::uint8_t blocker_index = blockers.bitscan(d);

                                    attacks.board &= ~movegen_rays[d][blocker_index].board;
                                }
                            }
                        }
                        break;

                    case Piece::Rook:
                        {
                            for (std::uint8_t d = 1; d < 8; d+=2)
                            {
                                attacks.board |= movegen_rays[d][index].board;

                                Bitboard blockers;
                                blockers.board = movegen_rays[d][index].board & all_blockers.board;
                                if (blockers.board != 0)
                                {
                                    std::uint8_t blocker_index = blockers.bitscan(d);

                                    attacks.board &= ~movegen_rays[d][blocker_index].board;
                                }
                            }
                        }
                        break;

                    case Piece::Queen:
                        {
                            for (std::uint8_t d = 0; d < 8; d++)
                            {
                                attacks.board |= movegen_rays[d][index].board;

                                Bitboard blockers;
                                blockers.board = movegen_rays[d][index].board & all_blockers.board;
                                if (blockers.board != 0)
                                {
                                    std::uint8_t blocker_index = blockers.bitscan(d);

                                    attacks.board &= ~movegen_rays[d][blocker_index].board;
                                }
                            }
                        }
                        break;

                    case Piece::King:
                        {
                            attacks.board = movegen_rays[static_cast<std::uint8_t>(Ray::King)][index].board;
                            king_index = index;
                        }
                        break;

                    default:
                        break;
                }

                if (tile.color == turn)
                    ray_movelist[index].board = attacks.board & (~colors[static_cast<std::uint8_t>(turn)].board);
                else
                    enemy_threat.board |= attacks.board;
            }
        }

        // Pawn non-attacking moves
        for (std::uint8_t x = 0; x < 8; x++)
        {
            for (std::uint8_t y = 0; y < 8; y++)
            {
                const Tile tile = get_tile(x, y);

                std::uint8_t index = y*8+x;

                if (tile.piece == Piece::Pawn && tile.color == turn)
                {
                    if (turn == Color::White)
                    {
                        if (!all_blockers.read(x, y+1))
                        {
                            ray_movelist[index].write(x, y+1, true);
                            if (y == 1)
                                ray_movelist[index].write(x, y+2, !all_blockers.read(x, y+2));
                        }
                    }
                    else
                    {
                        if (!all_blockers.read(x, y-1))
                        {
                            ray_movelist[index].write(x, y-1, true);
                            if (y == 6)
                                ray_movelist[index].write(x, y-2, !all_blockers.read(x, y-2));
                        }
                    }
                }
            }
        }

        // Castling moves
        if (turn == Color::White)
        {
            if (can_castle.at(static_cast<std::uint8_t>(Color::White)).at(0)) // King side
            {
                Bitboard must_be_clear;
                must_be_clear.write(5, 0, true);
                must_be_clear.write(6, 0, true);

                if ((must_be_clear.board & all_blockers.board) == 0)
                {
                    ray_movelist[king_index].write(6, 0, true);
                }
            }

            if (can_castle.at(static_cast<std::uint8_t>(Color::White)).at(1)) // Queen side
            {
                Bitboard must_be_clear;
                must_be_clear.write(1, 0, true);
                must_be_clear.write(2, 0, true);
                must_be_clear.write(3, 0, true);

                if ((must_be_clear.board & all_blockers.board) == 0)
                {
                    ray_movelist[king_index].write(2, 0, true);
                }
            }
        }
        else if (turn == Color::Black)
        {
            if (can_castle.at(static_cast<std::uint8_t>(Color::White)).at(0)) // King side
            {
                Bitboard must_be_clear;
                must_be_clear.write(5, 7, true);
                must_be_clear.write(6, 7, true);

                if ((must_be_clear.board & all_blockers.board) == 0)
                {
                    ray_movelist[king_index].write(6, 7, true);
                }
            }

            if (can_castle.at(static_cast<std::uint8_t>(Color::White)).at(1)) // Queen side
            {
                Bitboard must_be_clear;
                must_be_clear.write(1, 7, true);
                must_be_clear.write(2, 7, true);
                must_be_clear.write(3, 7, true);

                if ((must_be_clear.board & all_blockers.board) == 0)
                {
                    ray_movelist[king_index].write(2, 7, true);
                }
            }
        }

        if (!only_threat)
        {
            // Remove moves where king moves into enemy threat
            ray_movelist[king_index].board &= ~enemy_threat.board;

            bool in_check = enemy_threat.read(king_index%8, king_index/8);

            for (std::uint8_t x = 0; x < 8; x++)
            {
                for (std::uint8_t y = 0; y < 8; y++)
                {
                    std::uint8_t index = y*8+x;
                    if (ray_movelist[index].board == 0)
                        continue;

                    const Tile tile = get_tile(x, y);

                    // Check all moves if the king is in check,
                    // check moves whose tile is under threat (can be discovers),
                    // check moves that can be en passant as en passant must always be checked
                    if (in_check || enemy_threat.read(x, y) || (tile.piece == Piece::Pawn))
                    {
                        Bitboard moves = ray_movelist[index];

                        while (moves.board != 0)
                        {
                            std::uint8_t to_index = moves.bitscan_forward();

                            std::uint8_t to_x = to_index%8;
                            std::uint8_t to_y = to_index/8;
                            moves.write(to_x, to_y, false);

                            // Don't check pawn moves not under threat and not en passant
                            if ((!enemy_threat.read(x, y)) && tile.piece == Piece::Pawn && to_x == x)
                                continue;

                            Board next(*this);

                            next.perform_move(Move(x, y, to_x, to_y));

                            next.ray_movegen(true);
                            Bitboard next_threat = next.get_threat();

                            if ((next_threat.board & get_bitboard(turn, Piece::King).board) != 0)
                            {
                                ray_movelist[index].write(to_x, to_y, false);
                            }
                        }
                    }
                }
            }
        }

        movelist_found = true;
        return ray_movelist;
    }

    std::vector<Move> get_moves() const
    {
        //find_movelist();
        ray_movegen();

        std::uint8_t move_count = 0;

        for (std::uint8_t i = 0; i < 64; i++)
        {
            move_count += ray_movelist[i].count();
        }

        std::vector<Move> movelist_vec;
        movelist_vec.reserve(move_count);

        std::array<Bitboard, 64> ray_movelist_copy = ray_movelist;

        for (std::uint8_t x = 0; x < 8; x++)
        {
            for (std::uint8_t y = 0; y < 8; y++)
            {
                std::uint8_t index = y*8+x;

                while (ray_movelist_copy[index].board != 0)
                {
                    const Tile tile = get_tile(x, y);

                    std::uint8_t to_index = ray_movelist_copy[index].bitscan_forward();

                    std::uint8_t to_x = to_index%8;
                    std::uint8_t to_y = to_index/8;
                    ray_movelist_copy[index].write(to_x, to_y, false);

                    if (tile.piece == Piece::Pawn && ((to_y == 0) || (to_y == 7)))
                    {
                        movelist_vec.emplace_back(x, y, to_x, to_y, Piece::Knight);
                        movelist_vec.emplace_back(x, y, to_x, to_y, Piece::Bishop);
                        movelist_vec.emplace_back(x, y, to_x, to_y, Piece::Rook);
                        movelist_vec.emplace_back(x, y, to_x, to_y, Piece::Queen);
                    }
                    else
                    {
                        movelist_vec.emplace_back(x, y, to_x, to_y);
                    }
                }
            }
        }

        return movelist_vec;
    }

    Bitboard& get_threat() const
    {
        find_movelist(true);

        return threat;
    }

    Bitboard& get_enemy_threat() const
    {
        if (enemy_threat.board != 0)
            return enemy_threat;

        Board oppo(*this);

        Color enemy_turn = Color::White;
        if (turn == Color::White)
            enemy_turn = Color::Black;

        oppo.set_turn(enemy_turn);
        enemy_threat = oppo.get_threat();

        return enemy_threat;
    }

    void set_turn(Color color)
    {
        turn = color;

        movelist.clear();
        movelist_found = false;
        threat.board = 0;
        enemy_threat.board = 0;
    }

    Color get_turn() const
    {
        return turn;
    }

    void perform_move(Move move)
    {
        Tile from = get_tile(move.fx, move.fy);
        //Tile to = get_tile(move.tx, move.ty);

        if (move.promo == Piece::None)
            set_tile(move.tx, move.ty, from);
        else
            set_tile(move.tx, move.ty, Tile{from.color, move.promo});

        set_tile(move.fx, move.fy, Tile{Color::Empty, Piece::None});

        // Castling move
        if (from.piece == Piece::King && std::abs(move.tx - move.fx) >= 2)
        {
            // Kingside
            if (move.tx > move.fx)
            {
                set_tile(5, move.fy, Tile{from.color, Piece::Rook});
                set_tile(7, move.fy, Tile{Color::Empty, Piece::None});
            }

            // Queenside
            if (move.tx < move.fx)
            {
                set_tile(3, move.fy, Tile{from.color, Piece::Rook});
                set_tile(0, move.fy, Tile{Color::Empty, Piece::None});
            }
        }

        // Handle castling priviledges if king move
        if (from.piece == Piece::King)
        {
            can_castle.at(static_cast<std::uint8_t>(turn)).at(0) = false;
            can_castle.at(static_cast<std::uint8_t>(turn)).at(1) = false;
        }

        // Handle castling priviledges if rook move
        if (from.piece == Piece::Rook &&
                ((turn == Color::White && move.fy == 0) ||
                 (turn == Color::Black && move.fy == 7)))
        {
            if (move.fx == 7)
            {
                can_castle.at(static_cast<std::uint8_t>(turn)).at(0) = false;
            }

            if (move.fx == 0)
            {
                can_castle.at(static_cast<std::uint8_t>(turn)).at(1) = false;
            }
        }

        // Handle castling if rook is captured
        if (move.tx == 7 && move.ty == 0)
            can_castle.at(static_cast<std::uint8_t>(Color::White)).at(0) = false;
        if (move.tx == 0 && move.ty == 0)
            can_castle.at(static_cast<std::uint8_t>(Color::White)).at(1) = false;
        if (move.tx == 7 && move.ty == 7)
            can_castle.at(static_cast<std::uint8_t>(Color::Black)).at(0) = false;
        if (move.tx == 0 && move.ty == 7)
            can_castle.at(static_cast<std::uint8_t>(Color::Black)).at(1) = false;

        // En passant
        if (from.piece == Piece::Pawn &&
                move.fx != move.tx &&
                move.tx == ep_x)
        {
            if ((turn == Color::White && move.ty == 5) ||
                (turn == Color::Black && move.ty == 2))
            set_tile(ep_x, move.fy, Tile{Color::Empty, Piece::None});
        }

        // En passantable move
        if (from.piece == Piece::Pawn && std::abs(move.ty - move.fy) == 2)
        {
            ep_x = move.fx;
        }
        else
        {
            ep_x = 9;
        }

        if (get_turn() == Color::White)
            set_turn(Color::Black);
        else
            set_turn(Color::White);
    }

    bool is_checkmate() const
    {
        find_movelist();

        if (movelist.size() != 0)
            return false;

        get_enemy_threat();

        if ((enemy_threat.board & get_bitboard(turn, Piece::King).board) != 0)
            return true;

        return false;
    }

    bool is_stalemate() const
    {
        find_movelist();

        if ((movelist.size() != 0) || is_checkmate())
            return false;

        get_enemy_threat();

        if ((enemy_threat.board & get_bitboard(turn, Piece::King).board) == 0)
            return true;

        return false;
    }

    double basic_eval() const
    {
        find_movelist();

        if (movelist.size() == 0)
        {
            if (is_stalemate())
                return 0;
            if (turn == Color::White)
                //return -std::numeric_limits<double>::infinity();
                return -200.00;
            if (turn == Color::Black)
                return 200.00;
                //return std::numeric_limits<double>::infinity();
        }

        double eval = 0;

        for (std::uint8_t x = 0; x < 8; x++)
        {
            for (std::uint8_t y = 0; y < 8; y++)
            {
                Tile t = get_tile(x, y);

                double pv = 0;

                switch (t.piece)
                {
                    case Piece::None:   pv = 0; break;
                    case Piece::Pawn:   pv = 1; break;
                    case Piece::Knight: pv = 3; break;
                    case Piece::Bishop: pv = 3; break;
                    case Piece::Rook:   pv = 5; break;
                    case Piece::Queen:  pv = 9; break;
                    default:            pv = 0; break;
                }

                if (t.color == Color::White)
                    eval += pv;
                else if (t.color == Color::Black)
                    eval -= pv;
            }
        }

        return eval;
    }

    // Created based on "Simplified Evalution Function" on Chess Programming Wiki
    double adv_eval() const
    {
        // Piece values
        constexpr std::array<double, 6> piece_values = {1.00, 3.20, 3.30, 5.00, 9.00, 200.00};

        // Pawn Piece-Square Table
        constexpr std::array<double, 64> pawn_ps =
        {
             0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00,
             0.50, 0.50, 0.50, 0.50, 0.50, 0.50, 0.50, 0.50,
             0.10, 0.10, 0.20, 0.30, 0.30, 0.20, 0.10, 0.10,
             0.05, 0.05, 0.10, 0.25, 0.25, 0.10, 0.05, 0.05,
             0.00, 0.00, 0.00, 0.20, 0.20, 0.00, 0.00, 0.00,
             0.05,-0.05,-0.10, 0.00, 0.00,-0.10,-0.05, 0.05,
             0.05, 0.10, 0.10,-0.20,-0.20, 0.10, 0.10, 0.05,
             0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00
        };

        // Knight Piece-Square Table
        constexpr std::array<double, 64> knight_ps =
        {
            -0.50,-0.40,-0.30,-0.30,-0.30,-0.30,-0.40,-0.50,
            -0.40,-0.20, 0.00, 0.00, 0.00, 0.00,-0.20,-0.40,
            -0.30, 0.00, 0.10, 0.15, 0.15, 0.10, 0.00,-0.30,
            -0.30, 0.05, 0.15, 0.20, 0.20, 0.15, 0.05,-0.30,
            -0.30, 0.00, 0.15, 0.20, 0.20, 0.15, 0.00,-0.30,
            -0.30, 0.05, 0.10, 0.15, 0.15, 0.10, 0.05,-0.30,
            -0.40,-0.20, 0.00, 0.05, 0.05, 0.00,-0.20,-0.40,
            -0.50,-0.40,-0.30,-0.30,-0.30,-0.30,-0.40,-0.50
        };

        // Bishop Piece-Square Table
        constexpr std::array<double, 64> bishop_ps =
        {
            -0.20,-0.10,-0.10,-0.10,-0.10,-0.10,-0.10,-0.20,
            -0.10, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00,-0.10,
            -0.10, 0.00, 0.05, 0.10, 0.10, 0.05, 0.00,-0.10,
            -0.10, 0.05, 0.05, 0.10, 0.10, 0.05, 0.05,-0.10,
            -0.10, 0.00, 0.10, 0.10, 0.10, 0.10, 0.00,-0.10,
            -0.10, 0.10, 0.10, 0.10, 0.10, 0.10, 0.10,-0.10,
            -0.10, 0.05, 0.00, 0.00, 0.00, 0.00, 0.05,-0.10,
            -0.20,-0.10,-0.10,-0.10,-0.10,-0.10,-0.10,-0.20
        };

        // Rook Piece-Square Table
        constexpr std::array<double, 64> rook_ps =
        {
             0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00,
             0.05, 0.10, 0.10, 0.10, 0.10, 0.10, 0.10, 0.05,
            -0.05, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00,-0.05,
            -0.05, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00,-0.05,
            -0.05, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00,-0.05,
            -0.05, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00,-0.05,
            -0.05, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00,-0.05,
             0.00, 0.00, 0.00, 0.05, 0.05, 0.00, 0.00, 0.00
        };

        // Queen Piece-Square Table
        constexpr std::array<double, 64> queen_ps =
        {
            -0.20,-0.10,-0.10,-0.05,-0.05,-0.10,-0.10,-0.20,
            -0.10, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00,-0.10,
            -0.10, 0.00, 0.05, 0.05, 0.05, 0.05, 0.00,-0.10,
            -0.05, 0.00, 0.05, 0.05, 0.05, 0.05, 0.00,-0.05,
             0.00, 0.00, 0.05, 0.05, 0.05, 0.05, 0.00,-0.05,
            -0.10, 0.05, 0.05, 0.05, 0.05, 0.05, 0.00,-0.10,
            -0.10, 0.00, 0.05, 0.00, 0.00, 0.00, 0.00,-0.10,
            -0.20,-0.10,-0.10,-0.05,-0.05,-0.10,-0.10,-0.20
        };

        // King middle-game Piece-Square Table
        constexpr std::array<double, 64> king_middle_ps =
        {
            -0.30,-0.40,-0.40,-0.50,-0.50,-0.40,-0.40,-0.30,
            -0.30,-0.40,-0.40,-0.50,-0.50,-0.40,-0.40,-0.30,
            -0.30,-0.40,-0.40,-0.50,-0.50,-0.40,-0.40,-0.30,
            -0.30,-0.40,-0.40,-0.50,-0.50,-0.40,-0.40,-0.30,
            -0.20,-0.30,-0.30,-0.40,-0.40,-0.30,-0.30,-0.20,
            -0.10,-0.20,-0.20,-0.20,-0.20,-0.20,-0.20,-0.10,
             0.20, 0.20, 0.00, 0.00, 0.00, 0.00, 0.20, 0.20,
             0.20, 0.30, 0.10, 0.00, 0.00, 0.10, 0.30, 0.20
        };

        // King end-game Piece-Square Table
        constexpr std::array<double, 64> king_end_ps =
        {
            -0.50,-0.40,-0.30,-0.20,-0.20,-0.30,-0.40,-0.50,
            -0.30,-0.20,-0.10, 0.00, 0.00,-0.10,-0.20,-0.30,
            -0.30,-0.10, 0.20, 0.30, 0.30, 0.20,-0.10,-0.30,
            -0.30,-0.10, 0.30, 0.40, 0.40, 0.30,-0.10,-0.30,
            -0.30,-0.10, 0.30, 0.40, 0.40, 0.30,-0.10,-0.30,
            -0.30,-0.10, 0.20, 0.30, 0.30, 0.20,-0.10,-0.30,
            -0.30,-0.30, 0.00, 0.00, 0.00, 0.00,-0.30,-0.30,
            -0.50,-0.30,-0.30,-0.30,-0.30,-0.30,-0.30,-0.50
        };

        find_movelist();

        if (is_stalemate())
            return 0;

        if (is_checkmate())
        {
            if (turn == Color::White)
                return -200.00;

            if (turn == Color::Black)
                return 200.00;
        }

        double eval = 0;

        // 0 is middle game, 1 is end game, can interpolate between
        double endgameness = 0;

        {
            Bitboard white_pieces;
            Bitboard black_pieces;
            white_pieces.board = colors.at(static_cast<std::uint8_t>(Color::White)).board - get_bitboard(Color::White, Piece::Pawn).board;
            black_pieces.board = colors.at(static_cast<std::uint8_t>(Color::Black)).board - get_bitboard(Color::Black, Piece::Pawn).board;

            if (
                    ((get_bitboard(Color::White, Piece::Queen).board == 0) || (white_pieces.count() <= 1)) ||
                    ((get_bitboard(Color::Black, Piece::Queen).board == 0) || (black_pieces.count() <= 1))
               )
            {
                endgameness = 1;
            }
        }

        for (std::uint8_t x = 0; x < 8; x++)
        {
            for (std::uint8_t y = 0; y < 8; y++)
            {
                Tile t = get_tile(x, y);

                if (t.piece == Piece::None)
                    continue;

                std::uint8_t index = (7-y)*8+x; // For Piece-Square Tables for white
                if (t.color == Color::Black)
                    index = y*8+x; // For black (reflected, not rotated)

                double pv = piece_values.at(static_cast<std::uint8_t>(t.piece));

                switch (t.piece)
                {
                    case Piece::Pawn:   pv += pawn_ps.at(index); break;
                    case Piece::Knight: pv += knight_ps.at(index); break;
                    case Piece::Bishop: pv += bishop_ps.at(index); break;
                    case Piece::Rook:   pv += rook_ps.at(index); break;
                    case Piece::Queen:  pv += queen_ps.at(index); break;
                    case Piece::King:   pv += endgameness*king_end_ps.at(index) + (1-endgameness)*king_middle_ps.at(index); break;
                    default: break;
                }

                if (t.color == Color::White)
                    eval += pv;
                else
                    eval -= pv;
            }
        }

        return eval;
    }

    void print(std::ostream &os = std::cout) const
    {
        std::string s(10*11, ' ');

        for (std::uint8_t i = 0; i < 10; i++)
            s.at(i*11+10) = '\n';

        for (std::uint8_t x = 0; x < 8; x++)
        {
            for (std::uint8_t y = 0; y < 8; y++)
            {
                std::uint8_t pos = ((8-y)*11)+x+1;

                s.at(pos) = tile_to_char(get_tile(x, y));
            }
        }

        if (turn == Color::White)
            os << "White to move\n";
        else
            os << "Black to move\n";

        os << s;

        /*
        std::cout << "Colors (white/black/empty)\n";

        for (std::uint8_t c = 0; c < 3; c++)
        {
            colors.at(c).print();
            std::cout << '\n';
        }

        std::cout << "Pieces (p,k,b,r,q,k)\n";

        for (std::uint8_t p = 0; p < 6; p++)
        {
            pieces.at(p).print();
            std::cout << '\n';
        }
        */

        os << "Can castle: ";
        if (can_castle.at(0).at(0))
            os << 'K';
        else
            os << '-';

        if (can_castle.at(0).at(1))
            os << 'Q';
        else
            os << '-';

        if (can_castle.at(1).at(0))
            os << 'k';
        else
            os << '-';

        if (can_castle.at(1).at(1))
            os << 'q';
        else
            os << '-';

        os << std::endl;
    }

private:
    void find_movelist(bool find_threat = false) const
    {
        if (movelist_found)
            return;

        if (find_threat && threat.board != 0)
            return;

        Color player = get_turn();
        Color enemy = Color::White;
        if (player == Color::White)
            enemy = Color::Black;

        if (find_threat)
        {
            threat.board = 0;
        }
        else
        {
            enemy_threat = get_enemy_threat();
        }

        std::vector<Move> moves; // This will include pseudolegal moves

        // For every tile ...
        for (std::uint8_t x = 0; x < 8; x++)
        {
            for (std::uint8_t y = 0; y < 8; y++)
            {
                const Tile tile = get_tile(x, y);

                // Check if the piece is correct color
                if (player != tile.color)
                {
                    continue;
                }

                // Quick method to expand pawn moves into all promotions
                const auto pawn_moves = [&](std::uint8_t fx, std::uint8_t fy, std::uint8_t tx, std::uint8_t ty)
                {
                    if (!find_threat)
                    {
                        if (ty == 7 || ty == 0)
                        {
                            moves.emplace_back(fx, fy, tx, ty, Piece::Knight);
                            moves.emplace_back(fx, fy, tx, ty, Piece::Bishop);
                            moves.emplace_back(fx, fy, tx, ty, Piece::Rook);
                            moves.emplace_back(fx, fy, tx, ty, Piece::Queen);
                        }
                        else
                        {
                            moves.emplace_back(fx, fy, tx, ty);
                        }
                    }
                };

                // Quick method for iterating over knight moves
                const auto km = [](std::uint8_t i, std::uint8_t n)
                {
                    if (n == 0)
                    {
                        switch (i)
                        {
                            case 0: return -1; break;
                            case 1: return  1; break;
                            case 2: return  2; break;
                            case 3: return  2; break;
                            case 4: return  1; break;
                            case 5: return -1; break;
                            case 6: return -2; break;
                            case 7: return -2; break;
                            default: return 0; break;
                        }
                    }
                    else
                    {
                        switch (i)
                        {
                            case 0: return  2; break;
                            case 1: return  2; break;
                            case 2: return  1; break;
                            case 3: return -1; break;
                            case 4: return -2; break;
                            case 5: return -2; break;
                            case 6: return -1; break;
                            case 7: return  1; break;
                            default: return 0; break;
                        }
                    }
                };

                // Quick method for iterating over bishop moves
                const auto bm = [](std::uint8_t d, std::uint8_t n)
                {
                    if (n == 0)
                    {
                        switch (d)
                        {
                            case 0: return  1; break;
                            case 1: return  1; break;
                            case 2: return -1; break;
                            case 3: return -1; break;
                            default: std::cout << "??" << std::endl; return 1; break;
                        }
                    }
                    else if (n == 1)
                    {
                        switch (d)
                        {
                            case 0: return  1; break;
                            case 1: return -1; break;
                            case 2: return  1; break;
                            case 3: return -1; break;
                            default: std::cout << "??" << std::endl; return 1; break;
                        }
                    }
                    else
                    {
                        std::cout << "!?!" << std::endl;
                        return 1;
                    }
                };

                // Quick method for iterating over rook moves
                const auto rm = [](std::uint8_t d, std::uint8_t n)
                {
                    switch (d)
                    {
                        case 0: return n ?  1 :  0; break;
                        case 1: return n ?  0 :  1; break;
                        case 2: return n ? -1 :  0; break;
                        case 3: return n ?  0 : -1; break;
                        default: return 1; break;
                    }
                };

                switch (tile.piece)
                {
                    case Piece::Pawn:
                        {
                            if (player == Color::White)
                            {
                                // Standard pawn move
                                if (get_tile(x, y+1).color == Color::Empty)
                                {
                                    pawn_moves(x, y, x, y+1);

                                    // If at starting pos
                                    if (y == 1 && get_tile(x, y+2).color == Color::Empty)
                                    {
                                        pawn_moves(x, y, x, y+2);
                                    }
                                }

                                // Attacking moves
                                {
                                    const Tile t1 = get_tile(x-1, y+1);
                                    if (!t1.oob)
                                    {
                                        threat.write(x-1, y+1, true);

                                        if ((t1.color == enemy) ||
                                                (y == 4 && ep_x == x-1)) // En passant
                                        {
                                            pawn_moves(x, y, x-1, y+1);
                                        }
                                    }

                                    const Tile t2 = get_tile(x+1, y+1);
                                    if (!t2.oob)
                                    {
                                        threat.write(x+1, y+1, true);

                                        if ((t2.color == enemy) ||
                                                (y == 4 && ep_x == x+1)) // En passant
                                        {
                                            pawn_moves(x, y, x+1, y+1);
                                        }
                                    }
                                }
                            }
                            else
                            {
                                // Standard pawn move
                                if (get_tile(x, y-1).color == Color::Empty)
                                {
                                    pawn_moves(x, y, x, y-1);

                                    // If at starting pos
                                    if (y == 6 && get_tile(x, y-2).color == Color::Empty)
                                    {
                                        pawn_moves(x, y, x, y-2);
                                    }
                                }

                                // Attacking moves
                                {
                                    const Tile t1 = get_tile(x-1, y-1);
                                    if (!t1.oob)
                                    {
                                        threat.write(x-1, y-1, true);

                                        if ((t1.color == enemy) ||
                                                (y == 3 && ep_x == x-1)) // En passant
                                        {
                                            pawn_moves(x, y, x-1, y-1);
                                        }
                                    }

                                    const Tile t2 = get_tile(x+1, y-1);
                                    if (!t2.oob)
                                    {
                                        threat.write(x+1, y-1, true);

                                        if ((t2.color == enemy) ||
                                                (y == 3 && ep_x == x+1)) // En passant
                                        {
                                            pawn_moves(x, y, x+1, y-1);
                                        }
                                    }
                                }
                            }
                        }
                        break;

                    case Piece::Knight:
                        {
                            for (std::uint8_t i = 0; i < 8; i++)
                            {
                                std::int8_t x_ = x+km(i,0);
                                std::int8_t y_ = y+km(i,1);

                                Tile t = get_tile(x_, y_);
                                if (!t.oob)
                                {
                                    threat.write(x_, y_, true);

                                    if (t.color != player)
                                    {
                                        moves.emplace_back(x, y, x_, y_);
                                    }
                                }
                            }
                        }
                        break;

                    case Piece::Bishop:
                        {
                            for (std::uint8_t d = 0; d < 4; d++)
                            {
                                for (std::uint8_t i = 1; ; i++)
                                {
                                    std::int8_t x_ = x+i*bm(d,0);
                                    std::int8_t y_ = y+i*bm(d,1);

                                    Tile t = get_tile(x_, y_);

                                    //if (player == Color::White)
                                        //std::cout << "\n. " << std::to_string(x_) << " " << std::to_string(y_);

                                    if (t.oob)
                                        break;

                                    threat.write(x_, y_, true);

                                    if (t.color == player)
                                        break;

                                    //if (player == Color::White)
                                        //std::cout << '!';

                                    moves.emplace_back(x, y, x_, y_);

                                    if (t.color == enemy)
                                        break;

                                    //if (player == Color::White)
                                        //std::cout << '?';
                                }
                            }
                        }
                        break;

                    case Piece::Rook:
                        {
                            for (std::uint8_t d = 0; d < 4; d++)
                            {
                                for (std::uint8_t i = 1; ; i++)
                                {
                                    std::int8_t x_ = x+i*rm(d,0);
                                    std::int8_t y_ = y+i*rm(d,1);

                                    Tile t = get_tile(x_, y_);

                                    if (t.oob)
                                        break;

                                    threat.write(x_, y_, true);

                                    if (t.color == player)
                                        break;

                                    moves.emplace_back(x, y, x_, y_);

                                    if (t.color == enemy)
                                        break;
                                }
                            }
                        }
                        break;

                    case Piece::Queen:
                        {
                            // Bishop moves
                            for (std::uint8_t d = 0; d < 4; d++)
                            {
                                for (std::uint8_t i = 1; ; i++)
                                {
                                    std::int8_t x_ = x+i*bm(d,0);
                                    std::int8_t y_ = y+i*bm(d,1);

                                    Tile t = get_tile(x_, y_);

                                    if (t.oob)
                                        break;

                                    threat.write(x_, y_, true);

                                    if (t.color == player)
                                        break;

                                    moves.emplace_back(x, y, x_, y_);

                                    if (t.color == enemy)
                                        break;
                                }
                            }

                            // Rook moves
                            for (std::uint8_t d = 0; d < 4; d++)
                            {
                                for (std::uint8_t i = 1; ; i++)
                                {
                                    std::int8_t x_ = x+i*rm(d,0);
                                    std::int8_t y_ = y+i*rm(d,1);

                                    Tile t = get_tile(x_, y_);

                                    if (t.oob)
                                        break;

                                    threat.write(x_, y_, true);

                                    if (t.color == player)
                                        break;

                                    moves.emplace_back(x, y, x_, y_);

                                    if (t.color == enemy)
                                        break;
                                }
                            }
                        }
                        break;

                    case Piece::King:
                        {
                            for (std::int8_t x_ = x-1; x_ <= x+1; x_++)
                            {
                                for (std::int8_t y_ = y-1; y_ <= y+1; y_++)
                                {
                                    if (x_ == x && y_ == y)
                                        continue;

                                    Tile t = get_tile(x_, y_);

                                    if (t.oob)
                                        continue;

                                    threat.write(x_, y_, true);

                                    if (t.color == player)
                                        continue;

                                    moves.emplace_back(x, y, x_, y_);
                                }
                            }

                            //King threatens his own square (important I suspect for checking if enemy threat has been found)
                            threat.write(x, y, true);

                            // Castling moves don't threat
                            if (!find_threat)
                            {
                                // Kingside castling
                                if (can_castle.at(static_cast<std::uint8_t>(player)).at(0))
                                {
                                    bool clear = true;

                                    for (std::uint8_t x_ = 6; x_ >= 5; x_--)
                                    {
                                        if (get_tile(x_, y).color != Color::Empty)
                                        {
                                            clear = false;
                                            break;
                                        }
                                    }

                                    if (clear)
                                    {
                                        Bitboard king_squares;
                                        king_squares.write(4, y, true);
                                        king_squares.write(5, y, true);
                                        king_squares.write(6, y, true);

                                        if ((enemy_threat.board & king_squares.board) == 0)
                                        {
                                            moves.emplace_back(x, y, 6, y);
                                        }
                                    }
                                }

                                // Queenside castling
                                if (can_castle.at(static_cast<std::uint8_t>(player)).at(1))
                                {
                                    bool clear = true;

                                    for (std::uint8_t x_ = 1; x_ <= 3; x_++)
                                    {
                                        if (get_tile(x_, y).color != Color::Empty)
                                        {
                                            clear = false;
                                            break;
                                        }
                                    }

                                    if (clear)
                                    {
                                        Bitboard king_squares;
                                        king_squares.write(2, y, true);
                                        king_squares.write(3, y, true);
                                        king_squares.write(4, y, true);

                                        if ((enemy_threat.board & king_squares.board) == 0)
                                        {
                                            moves.emplace_back(x, y, 2, y);
                                        }
                                    }
                                }
                            }
                        }
                        break;

                    default:
                        {
                            std::cerr << "Unhandled piece type for finding move!" << std::endl;
                        }
                        break;
                }
            }
        }

        for (const Move &move : moves)
        {
            threat.write(move.tx, move.ty, true);
        }

        if (!find_threat)
        {
            Bitboard king_tile = get_bitboard(player, Piece::King);

            bool under_threat = static_cast<bool>(enemy_threat.board & king_tile.board);

            for (const Move &move : moves)
            {
                Piece from = get_piece(move.fx, move.fy);
                Bitboard tile_bb;
                tile_bb.write(move.fx, move.fy, true);

                // Remove moves where kings walks into check
                if (from == Piece::King)
                {
                    Bitboard king_move;
                    king_move.write(move.tx, move.ty, true);

                    if ((enemy_threat.board & king_move.board) != 0)
                    {
                        continue;
                    }
                }

                // Check moves that cannot be assumed safe (en passant must always be checked)
                if (under_threat ||
                        (enemy_threat.board & tile_bb.board) ||
                        (from == Piece::Pawn && (move.fx != move.tx)))
                {
                    Board next(*this);

                    next.perform_move(move);

                    Bitboard next_threat = next.get_threat();

                    if ((next.get_bitboard(player, Piece::King).board & next_threat.board) != 0)
                    {
                        continue;
                    }
                }

                movelist.push_back(move);
            }

            movelist_found = turn == player && !find_threat;
        }
    }

    std::array<Bitboard, 3> colors;
    std::array<Bitboard, 6> pieces;

    mutable Bitboard threat;
    mutable Bitboard enemy_threat;
    mutable bool movelist_found = false;
    mutable std::vector<Move> movelist;
    mutable std::array<Bitboard, 64> ray_movelist;

    Color turn = Color::White;
    std::array<std::array<bool, 2>, 2> can_castle; // KQkq
    std::uint8_t ep_x = 9; // x value for en passant, 9 if no en passant
};

#endif
