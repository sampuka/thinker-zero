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
        colors.at(static_cast<std::uint8_t>(Color::Empty)) = ~0;

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
            if (bitboard_read(colors[c], x, y))
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
            if (bitboard_read(pieces[p], x, y))
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
            bitboard_unset(colors[c], x, y);
        }

        for (std::uint8_t p = 0; p < 6; p++)
        {
            bitboard_unset(pieces[p], x, y);
        }

        bitboard_set(colors[static_cast<std::uint8_t>(tile.color)], x, y);

        if (tile.piece != Piece::None)
        {
            bitboard_set(pieces[static_cast<std::uint8_t>(tile.piece)], x, y);
        }
    }

    Bitboard get_bitboard(Color color, Piece piece) const
    {
        return colors[static_cast<std::uint8_t>(color)] & pieces[static_cast<std::uint8_t>(piece)];
    }

    void ray_movegen(bool only_threat = false) const
    {
        if (movelist_found)
            return;

        Bitboard all_blockers = ~colors[static_cast<std::uint8_t>(Color::Empty)];

        enemy_threat = 0;
        std::uint8_t king_index = 0;

        for (std::uint8_t x = 0; x < 8; x++)
        {
            for (std::uint8_t y = 0; y < 8; y++)
            {
                const Tile tile = get_tile(x, y);

                std::uint8_t index = y*8+x;

                Bitboard attacks = 0;

                switch (tile.piece)
                {
                    case Piece::Pawn: // Only attacking moves
                        {
                            if (tile.color == turn)
                            {
                                Bitboard enemy_pieces;
                                if (turn == Color::White)
                                    enemy_pieces = colors[static_cast<std::uint8_t>(Color::Black)];
                                else
                                    enemy_pieces = colors[static_cast<std::uint8_t>(Color::White)];

                                if (ep_x != 9)
                                {
                                    if (turn == Color::White)
                                        bitboard_set(enemy_pieces, ep_x, 5);
                                    else
                                        bitboard_set(enemy_pieces, ep_x, 2);
                                }

                                if (turn == Color::White)
                                {
                                    attacks = movegen_rays[static_cast<std::uint8_t>(Ray::WhitePawnAttacks)][index] & enemy_pieces;
                                }
                                else
                                {
                                    attacks = movegen_rays[static_cast<std::uint8_t>(Ray::BlackPawnAttacks)][index] & enemy_pieces;
                                }
                            }
                            else
                            {
                                if (tile.color == Color::White)
                                {
                                    attacks = movegen_rays[static_cast<std::uint8_t>(Ray::WhitePawnAttacks)][index];
                                }
                                else
                                {
                                    attacks = movegen_rays[static_cast<std::uint8_t>(Ray::BlackPawnAttacks)][index];
                                }
                            }
                        }
                        break;

                    case Piece::Knight:
                        {
                            attacks = movegen_rays[static_cast<std::uint8_t>(Ray::Knight)][index];
                        }
                        break;

                    case Piece::Bishop:
                        {
                            for (std::uint8_t d = 0; d < 8; d+=2)
                            {
                                attacks |= movegen_rays[d][index];

                                Bitboard blockers = movegen_rays[d][index] & all_blockers;
                                if (blockers != 0)
                                {
                                    std::uint8_t blocker_index = bitboard_bitscan(blockers, d);

                                    attacks &= ~movegen_rays[d][blocker_index];
                                }
                            }
                        }
                        break;

                    case Piece::Rook:
                        {
                            for (std::uint8_t d = 1; d < 8; d+=2)
                            {
                                attacks |= movegen_rays[d][index];

                                Bitboard blockers = movegen_rays[d][index] & all_blockers;
                                if (blockers != 0)
                                {
                                    std::uint8_t blocker_index = bitboard_bitscan(blockers, d);

                                    attacks &= ~movegen_rays[d][blocker_index];
                                }
                            }
                        }
                        break;

                    case Piece::Queen:
                        {
                            for (std::uint8_t d = 0; d < 8; d++)
                            {
                                attacks |= movegen_rays[d][index];

                                Bitboard blockers = movegen_rays[d][index] & all_blockers;
                                if (blockers != 0)
                                {
                                    std::uint8_t blocker_index = bitboard_bitscan(blockers, d);

                                    attacks &= ~movegen_rays[d][blocker_index];
                                }
                            }
                        }
                        break;

                    case Piece::King:
                        {
                            attacks = movegen_rays[static_cast<std::uint8_t>(Ray::King)][index];
                            if (tile.color == turn)
                                king_index = index;
                        }
                        break;

                    default:
                        break;
                }

                if (tile.color == turn)
                {
                    ray_movelist[index] = attacks & (~colors[static_cast<std::uint8_t>(turn)]);
                    threat |= attacks;
                }
                else
                {
                    enemy_threat |= attacks;
                }
            }
        }

        // Pawn moves and castles do not need to be found for threat
        if (!only_threat)
        {
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
                            if (!bitboard_read(all_blockers, x, y+1))
                            {
                                bitboard_set(ray_movelist[index], x, y+1);
                                if (y == 1 && !bitboard_read(all_blockers, x, y+2))
                                    bitboard_set(ray_movelist[index], x, y+2);
                            }
                        }
                        else
                        {
                            if (!bitboard_read(all_blockers, x, y-1))
                            {
                                bitboard_set(ray_movelist[index], x, y-1);
                                if (y == 6 && !bitboard_read(all_blockers, x, y-2))
                                    bitboard_set(ray_movelist[index], x, y-2);
                            }
                        }
                    }
                }
            }

            // Castling moves
            if (turn == Color::White)
            {
                if (can_castle[static_cast<std::uint8_t>(Color::White)][0]) // King side
                {
                    if (
                            (wks_clear & all_blockers) == 0 &&
                            (wks_safe & enemy_threat) == 0
                       )
                    {
                        bitboard_set(ray_movelist[king_index], 6, 0);
                    }
                }

                if (can_castle[static_cast<std::uint8_t>(Color::White)][1]) // Queen side
                {
                    if (
                            (wqs_clear & all_blockers) == 0 &&
                            (wqs_safe & enemy_threat) == 0
                       )
                    {
                        bitboard_set(ray_movelist[king_index], 2, 0);
                    }
                }
            }
            else if (turn == Color::Black)
            {
                if (can_castle[static_cast<std::uint8_t>(Color::Black)][0]) // King side
                {
                    if (
                            (bks_clear & all_blockers) == 0 &&
                            (bks_safe & enemy_threat) == 0
                       )
                    {
                        bitboard_set(ray_movelist[king_index], 6, 7);
                    }
                }

                if (can_castle[static_cast<std::uint8_t>(Color::Black)][1]) // Queen side
                {
                    if (
                            (bqs_clear & all_blockers) == 0 &&
                            (bqs_safe & enemy_threat) == 0
                       )
                    {
                        bitboard_set(ray_movelist[king_index], 2, 7);
                    }
                }
            }
        }

        if (!only_threat)
        {
            // Remove moves where king moves into enemy threat
            ray_movelist[king_index] &= ~enemy_threat;

            bool in_check = bitboard_read(enemy_threat, king_index);

            for (std::uint8_t x = 0; x < 8; x++)
            {
                for (std::uint8_t y = 0; y < 8; y++)
                {
                    std::uint8_t index = y*8+x;
                    if (ray_movelist[index] == 0)
                        continue;

                    const Tile tile = get_tile(x, y);

                    // Check all moves if the king is in check,
                    // check moves whose tile is under threat (can be discovers),
                    // check moves that can be en passant as en passant must always be checked
                    if (in_check || bitboard_read(enemy_threat, x, y) || (tile.piece == Piece::Pawn))
                    {
                        Bitboard moves = ray_movelist[index];

                        while (moves != 0)
                        {
                            std::uint8_t to_index = bitboard_bitscan_forward(moves);
                            bitboard_unset(moves, to_index);

                            std::uint8_t to_x = to_index%8;
                            std::uint8_t to_y = to_index/8;

                            // Don't check pawn moves not under threat and not en passant
                            if (
                                    !in_check &&
                                    ((!bitboard_read(enemy_threat, x, y)) && tile.piece == Piece::Pawn && to_x == x)
                               )
                                continue;

                            Board next(*this);

                            next.perform_move(Move(x, y, to_x, to_y));

                            Bitboard next_threat = next.get_threat();

                            if ((next_threat & next.get_bitboard(turn, Piece::King)) != 0)
                            {
                                bitboard_unset(ray_movelist[index], to_x, to_y);
                            }
                        }
                    }
                }
            }
        }

        // Don't create movelist if only finding threat
        std::uint8_t move_count = 0;
        if (!only_threat)
        {
            for (std::uint8_t i = 0; i < 64; i++)
            {
                move_count += bitboard_count(ray_movelist[i]);
            }

            movelist.clear();
            movelist.reserve(move_count);

            std::array<Bitboard, 64> ray_movelist_copy = ray_movelist;

            for (std::uint8_t x = 0; x < 8; x++)
            {
                for (std::uint8_t y = 0; y < 8; y++)
                {
                    std::uint8_t index = y*8+x;

                    while (ray_movelist_copy[index] != 0)
                    {
                        const Tile tile = get_tile(x, y);

                        std::uint8_t to_index = bitboard_bitscan_forward(ray_movelist_copy[index]);
                        bitboard_unset(ray_movelist_copy[index], to_index);

                        std::uint8_t to_x = to_index%8;
                        std::uint8_t to_y = to_index/8;

                        if (tile.piece == Piece::Pawn && ((to_y == 0) || (to_y == 7)))
                        {
                            movelist.emplace_back(x, y, to_x, to_y, Piece::Knight);
                            movelist.emplace_back(x, y, to_x, to_y, Piece::Bishop);
                            movelist.emplace_back(x, y, to_x, to_y, Piece::Rook);
                            movelist.emplace_back(x, y, to_x, to_y, Piece::Queen);
                        }
                        else
                        {
                            movelist.emplace_back(x, y, to_x, to_y);
                        }
                    }
                }
            }

            movelist_found = true;
        }

        return;
    }

    std::vector<Move>& get_moves() const
    {
        ray_movegen();

        return movelist;
    }

    Bitboard& get_threat() const
    {
        ray_movegen(true);

        return threat;
    }

    Bitboard& get_enemy_threat() const
    {
        ray_movegen(true);

        return enemy_threat;
    }

    void set_turn(Color color)
    {
        turn = color;

        movelist.clear();
        movelist_found = false;
        threat = 0;
        enemy_threat = 0;
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
        ray_movegen();

        if (movelist.size() != 0)
            return false;

        if ((enemy_threat & get_bitboard(turn, Piece::King)) != 0)
            return true;

        return false;
    }

    bool is_stalemate() const
    {
        ray_movegen();

        if ((movelist.size() != 0) || is_checkmate())
            return false;

        if ((enemy_threat & get_bitboard(turn, Piece::King)) == 0)
            return true;

        return false;
    }

    double basic_eval() const
    {
        ray_movegen();

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

        ray_movegen();

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
            Bitboard white_pieces = colors[static_cast<std::uint8_t>(Color::White)] - get_bitboard(Color::White, Piece::Pawn);
            Bitboard black_pieces = colors[static_cast<std::uint8_t>(Color::Black)] - get_bitboard(Color::Black, Piece::Pawn);

            if (
                    ((get_bitboard(Color::White, Piece::Queen) == 0) || (bitboard_count(white_pieces) <= 1)) ||
                    ((get_bitboard(Color::Black, Piece::Queen) == 0) || (bitboard_count(black_pieces) <= 1))
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

                double pv = piece_values[static_cast<std::uint8_t>(t.piece)];

                switch (t.piece)
                {
                    case Piece::Pawn:   pv += pawn_ps[index]; break;
                    case Piece::Knight: pv += knight_ps[index]; break;
                    case Piece::Bishop: pv += bishop_ps[index]; break;
                    case Piece::Rook:   pv += rook_ps[index]; break;
                    case Piece::Queen:  pv += queen_ps[index]; break;
                    case Piece::King:   pv += endgameness*king_end_ps[index] + (1-endgameness)*king_middle_ps[index]; break;
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
    std::array<Bitboard, 3> colors = {0};
    std::array<Bitboard, 6> pieces = {0};

    mutable Bitboard threat = 0;
    mutable Bitboard enemy_threat = 0;
    mutable bool movelist_found = false;
    mutable std::vector<Move> movelist;
    mutable std::array<Bitboard, 64> ray_movelist = {0};

    Color turn = Color::White;
    std::array<std::array<bool, 2>, 2> can_castle; // KQkq
    std::uint8_t ep_x = 9; // x value for en passant, 9 if no en passant
};

#endif
