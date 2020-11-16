#ifndef BOARD_HPP
#define BOARD_HPP

#include "utility.hpp"
#include "Bitboard.hpp"
#include "Move.hpp"
#include "movegen_rays.hpp"
#include "zobrist.hpp"

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
        turn(b.turn),
        can_castle(b.can_castle),
        ep_x(b.ep_x)
    {
    }

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

        repeatable_movecount = std::stoi(tokens.at(4));
        turn_number = std::stoi(tokens.at(5));
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

    Color get_color(Square sq) const
    {
        for (std::uint8_t c = 0; c < 3; c++)
        {
            if (bitboard_read(colors[c], sq))
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

    Piece get_piece(Square sq) const
    {
        for (std::uint8_t p = 0; p < 6; p++)
        {
            if (bitboard_read(pieces[p], sq))
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

    Tile get_tile(Square sq) const
    {
        return Tile{get_color(sq), get_piece(sq)};
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

    void set_tile(Square sq, Tile tile)
    {
        for (std::uint8_t c = 0; c < 3; c++)
        {
            bitboard_unset(colors[c], sq);
        }

        for (std::uint8_t p = 0; p < 6; p++)
        {
            bitboard_unset(pieces[p], sq);
        }

        bitboard_set(colors[static_cast<std::uint8_t>(tile.color)], sq);

        if (tile.piece != Piece::None)
        {
            bitboard_set(pieces[static_cast<std::uint8_t>(tile.piece)], sq);
        }
    }

    Bitboard get_bitboard(Color color, Piece piece) const
    {
        return colors[static_cast<std::uint8_t>(color)] & pieces[static_cast<std::uint8_t>(piece)];
    }

    /*
    bool is_checkmate(const MoveList& movelist) const
    {
        if (movelist.size() != 0)
            return false;

        if ((enemy_threat & get_bitboard(turn, Piece::King)) != 0)
            return true;

        return false;
    }

    bool is_stalemate(const MoveList& movelist) const
    {
        if (is_checkmate(movelist))
            return false;

        if (repeatable_movecount == 100)
            return true;

        if (movelist.size() != 0)
            return false;

        if ((enemy_threat & get_bitboard(turn, Piece::King)) == 0)
            return true;

        return false;
    }
    */

    void get_moves(MoveList& movelist) const
    {
        std::vector<std::uint64_t> z_list;
        get_moves(movelist, z_list);
    }

    void get_moves(MoveList& movelist, std::vector<std::uint64_t> z_list, bool debug = false) const
    {
        ray_movegen(movelist);

        std::uint64_t zob = get_zobrist();

        if (debug)
        {
            std::cout << zob << std::endl;
            for (std::uint64_t zo : z_list)
            {
                std::cout << zo << ' ';
            }
            std::cout << std::endl;
        }

        std::uint8_t count = 0;
        for (const std::uint64_t z : z_list)
        {
            if (z == zob)
            {
                count++;

                if (count == 3)
                {
                    if (debug && false)
                    {
                        std::cout << zob << std::endl;
                        for (std::uint64_t zo : z_list)
                        {
                            std::cout << zo << ' ';
                        }
                        std::cout << std::endl;
                    }
                    movelist.clear();
                    movelist.is_stalemate = true;
                    return;
                }
            }
        }

        if (movelist.size() == 0)
        {
            if (checkers == 0)
            {
                movelist.is_stalemate = true;
            }
            else if (checkers != 0)
            {
                movelist.is_checkmate = true;
            }
        }

        if (!movelist.is_checkmate && repeatable_movecount == 100)
        {
            movelist.is_stalemate = true;
        }
    }

    Bitboard& get_threat() const
    {
        static_analysis();

        return threat;
    }

    Bitboard& get_enemy_threat() const
    {
        static_analysis();

        return enemy_threat;
    }

    Bitboard& get_checkers() const
    {
        static_analysis();

        return checkers;
    }

    Bitboard& get_check_blockers() const
    {
        static_analysis();

        return check_blockers;
    }

    Bitboard& get_pinned() const
    {
        static_analysis();

        return pinned;
    }

    void set_turn(Color color)
    {
        turn = color;

        static_found = false;
        threat = 0;
        enemy_threat = 0;
        checkers = 0;
        check_blockers = 0;
        pinned = 0;
    }

    Color get_turn() const
    {
        return turn;
    }

    std::uint8_t get_ep() const
    {
        return ep_x;
    }

    void perform_move(Move move)
    {
        const Tile from = get_tile(move.get_from());

        const MoveSpecial move_type = move.get_type();
        const Square from_sq = move.get_from();
        const Square to_sq = move.get_to();

        const std::uint8_t fx = from_sq%8;
        const std::uint8_t fy = from_sq/8;
        const std::uint8_t tx = to_sq%8;
        const std::uint8_t ty = to_sq/8;

        movetohere = move;

        if (
                bitboard_read(~colors[static_cast<std::uint8_t>(Color::Empty)], to_sq) ||
                move_type == MoveSpecial::EnPassant
           )
        {
            typetohere = MoveType::Capture;

            repeatable_movecount = 0;
        }
        else
        {
            typetohere = MoveType::Quiet;

            if (from.piece == Piece::Pawn)
            {
                repeatable_movecount = 0;
            }
            else
            {
                repeatable_movecount++;
            }
        }

        if (move_type == MoveSpecial::Promotion)
            set_tile(to_sq, Tile{from.color, move.get_promo()});
        else
            set_tile(to_sq, from);

        set_tile(from_sq, Tile{Color::Empty, Piece::None});

        // Castling move
        if (move_type == MoveSpecial::Castling)
        {
            // Kingside
            if (tx > fx)
            {
                set_tile(5, fy, Tile{from.color, Piece::Rook});
                set_tile(7, fy, Tile{Color::Empty, Piece::None});
            }

            // Queenside
            if (tx < fx)
            {
                set_tile(3, fy, Tile{from.color, Piece::Rook});
                set_tile(0, fy, Tile{Color::Empty, Piece::None});
            }
        }

        // Handle castling priviledges if king move
        if (from.piece == Piece::King)
        {
            can_castle[static_cast<std::uint8_t>(turn)][0] = false;
            can_castle[static_cast<std::uint8_t>(turn)][1] = false;
        }

        // Handle castling priviledges if rook move
        if (from.piece == Piece::Rook &&
                ((turn == Color::White && fy == 0) ||
                 (turn == Color::Black && fy == 7)))
        {
            if (fx == 7)
            {
                can_castle[static_cast<std::uint8_t>(turn)][0] = false;
            }

            if (fx == 0)
            {
                can_castle[static_cast<std::uint8_t>(turn)][1] = false;
            }
        }

        // Handle castling if rook is captured
        if (tx == 7 && ty == 0)
            can_castle[static_cast<std::uint8_t>(Color::White)][0] = false;
        if (tx == 0 && ty == 0)
            can_castle[static_cast<std::uint8_t>(Color::White)][1] = false;
        if (tx == 7 && ty == 7)
            can_castle[static_cast<std::uint8_t>(Color::Black)][0] = false;
        if (tx == 0 && ty == 7)
            can_castle[static_cast<std::uint8_t>(Color::Black)][1] = false;

        // En passant
        if (move_type == MoveSpecial::EnPassant)
        {
            set_tile(ep_x, fy, Tile{Color::Empty, Piece::None});
        }

        // En passantable move
        if (from.piece == Piece::Pawn && std::abs(ty - fy) == 2)
        {
            ep_x = fx;
        }
        else
        {
            ep_x = 9;
        }

        if (get_turn() == Color::White)
        {
            set_turn(Color::Black);
        }
        else
        {
            turn_number++;
            set_turn(Color::White);
        }

        zobrist_hash = 0;
    }

    std::uint64_t get_zobrist() const
    {
        if (zobrist_hash != 0)
            return zobrist_hash;

        std::uint64_t z = 0;

        Bitboard all_pieces = colors[static_cast<std::uint8_t>(Color::White)] | colors[static_cast<std::uint8_t>(Color::Black)];

        while (all_pieces)
        {
            const Square sq = bitboard_bitscan_forward_pop(all_pieces);
            const Tile t = get_tile(sq);

            z ^= zobrist_pieces
                [static_cast<std::uint8_t>(t.color)] [static_cast<std::uint8_t>(t.piece)] [sq];
        }

        if (turn == Color::Black)
            z ^= zobrist_black;

        for (std::uint8_t s = 0; s < 2; s++)
        {
            for (std::uint8_t c = 0; c < 2; c++)
            {
                if (can_castle[c][s])
                    z ^= zobrist_castles[c][s];
            }
        }

        if (ep_x != 9)
            z ^= zobrist_ep[ep_x];

        zobrist_hash = z;

        return zobrist_hash;
    }

    double basic_eval(const MoveList& movelist) const
    {
        if (movelist.size() == 0)
        {
            if (movelist.is_stalemate)
                return 0;

            if (turn == Color::White)
                return -200.00;

            if (turn == Color::Black)
                return 200.00;
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
    double adv_eval(const MoveList& movelist) const
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

        if (movelist.is_stalemate)
            return 0;

        if (movelist.is_checkmate)
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
    void ray_movegen(MoveList& movelist) const
    {
        movelist.clear();

        static_analysis();

        Color their_color = Color::White;
        if (turn == Color::White)
            their_color = Color::Black;

        Square king_square = bitboard_bitscan_forward(get_bitboard(turn, Piece::King));
        Square their_king_square = bitboard_bitscan_forward(get_bitboard(their_color, Piece::King));

        const Bitboard all_blockers = ~colors[static_cast<std::uint8_t>(Color::Empty)];
        const Bitboard enemy_pieces = colors[static_cast<std::uint8_t>(their_color)];

        Bitboard it_pieces = colors[static_cast<std::uint8_t>(turn)];

        while (it_pieces)
        {
            const Square from_square = bitboard_bitscan_forward_pop(it_pieces);
            const std::uint8_t from_x = from_square%8;
            const std::uint8_t from_y = from_square/8;

            const Tile tile = get_tile(from_x, from_y);

            Bitboard attacks = 0;

            switch (tile.piece)
            {
                case Piece::Pawn:
                    {
                        Bitboard target = enemy_pieces;

                        if (turn == Color::White)
                        {
                            if (ep_x != 9)
                                bitboard_set(target, ep_x, 5);

                            attacks = movegen_rays[static_cast<std::uint8_t>(Ray::WhitePawnAttacks)][from_square] & target;
                        }
                        else
                        {
                            if (ep_x != 9)
                                bitboard_set(target, ep_x, 2);

                            attacks = movegen_rays[static_cast<std::uint8_t>(Ray::BlackPawnAttacks)][from_square] & target;
                        }
                    }
                    break;

                case Piece::Knight:
                    {
                        attacks = movegen_rays[static_cast<std::uint8_t>(Ray::Knight)][from_square];
                    }
                    break;

                case Piece::Bishop:
                    {
                        for (std::uint8_t d = 0; d < 8; d+=2)
                        {
                            attacks |= movegen_rays[d][from_square];

                            Bitboard blockers = movegen_rays[d][from_square] & all_blockers;

                            if (blockers != 0)
                            {
                                Square blocker_square = bitboard_bitscan(blockers, d);
                                attacks &= ~movegen_rays[d][blocker_square];
                            }
                        }
                    }
                    break;

                case Piece::Rook:
                    {
                        for (std::uint8_t d = 1; d < 8; d+=2)
                        {
                            attacks |= movegen_rays[d][from_square];

                            Bitboard blockers = movegen_rays[d][from_square] & all_blockers;

                            if (blockers != 0)
                            {
                                Square blocker_square = bitboard_bitscan(blockers, d);
                                attacks &= ~movegen_rays[d][blocker_square];
                            }
                        }
                    }
                    break;

                case Piece::Queen:
                    {
                        for (std::uint8_t d = 0; d < 8; d++)
                        {
                            attacks |= movegen_rays[d][from_square];

                            Bitboard blockers = movegen_rays[d][from_square] & all_blockers;

                            if (blockers != 0)
                            {
                                Square blocker_square = bitboard_bitscan(blockers, d);
                                attacks &= ~movegen_rays[d][blocker_square];
                            }
                        }
                    }
                    break;

                default:
                    break;
            }

            attacks &= ~colors[static_cast<std::uint8_t>(turn)];

            add_moves(movelist, from_square, attacks);
        }

        std::array<Bitboard, 2> king_threats =
        {
            movegen_rays[static_cast<std::uint8_t>(Ray::King)][king_square],
            movegen_rays[static_cast<std::uint8_t>(Ray::King)][their_king_square]
        };

        add_moves(movelist, king_square, king_threats[0] & (~(enemy_threat | king_threats[1] | colors[static_cast<std::uint8_t>(turn)])));

        // Pawn non-attacking moves
        Bitboard pawns = get_bitboard(turn, Piece::Pawn);
        while (pawns)
        {
            const Square sq = bitboard_bitscan_forward_pop(pawns);
            const std::uint8_t x = sq%8;
            const std::uint8_t y = sq/8;

            const Tile tile = get_tile(x, y);

            if (tile.piece == Piece::Pawn && tile.color == turn)
            {
                if (turn == Color::White)
                {
                    if (!bitboard_read(all_blockers, x, y+1))
                    {
                        add_move(movelist, Move(sq, (y+1)*8+x, MoveSpecial::None));
                        if (y == 1 && !bitboard_read(all_blockers, x, y+2))
                            add_move(movelist, Move(sq, (y+2)*8+x, MoveSpecial::None));
                    }
                }
                else
                {
                    if (!bitboard_read(all_blockers, x, y-1))
                    {
                        add_move(movelist, Move(sq, (y-1)*8+x, MoveSpecial::None));
                        if (y == 6 && !bitboard_read(all_blockers, x, y-2))
                            add_move(movelist, Move(sq, (y-2)*8+x, MoveSpecial::None));
                    }
                }
            }
        }

        if (checkers == 0) // Generate non-evasive
        {
            std::uint8_t list_size = movelist.size();
            movelist.clear();

            for (std::uint8_t i = 0; i < list_size; i++)
            {
                const Move& move = movelist.at(i);
                const Square from_sq = move.get_from();
                const Square to_sq = move.get_to();
                const std::uint8_t fx = from_sq%8;
                //const std::uint8_t fy = from_sq/8;
                const std::uint8_t tx = to_sq%8;
                const Tile tile = get_tile(from_sq);

                // Moving along pinned direction
                if (
                        bitboard_read(pinned, from_sq) &&
                        (
                         (tile.piece == Piece::Knight) ||
                         (!is_aligned(king_square, from_sq, to_sq))
                        )
                   )
                    continue;

                // En Passant edgecase
                if (
                        (tile.piece == Piece::Pawn) &&
                        (fx != tx) &&
                        !bitboard_read(all_blockers, to_sq)
                   )
                {
                    // Just gonna buteforce-check for now
                    Board next(*this);
                    next.perform_move(Move(from_sq, to_sq, MoveSpecial::EnPassant));

                    if (bitboard_read(next.get_threat(), king_square))
                        continue;

                    add_move(movelist, Move(from_sq, to_sq, MoveSpecial::EnPassant), true);
                }
                else
                {
                    add_move(movelist, move, true);
                }
            }

            // Generate castling moves
            if (turn == Color::White)
            {
                if (can_castle[static_cast<std::uint8_t>(Color::White)][0]) // King side
                {
                    if (
                            (wks_clear & all_blockers) == 0 &&
                            (wks_safe & enemy_threat) == 0
                       )
                    {
                        add_move(movelist, Move(king_square, 0*8+6, MoveSpecial::Castling), true);
                    }
                }

                if (can_castle[static_cast<std::uint8_t>(Color::White)][1]) // Queen side
                {
                    if (
                            (wqs_clear & all_blockers) == 0 &&
                            (wqs_safe & enemy_threat) == 0
                       )
                    {
                        add_move(movelist, Move(king_square, 0*8+2, MoveSpecial::Castling), true);
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
                        add_move(movelist, Move(king_square, 7*8+6, MoveSpecial::Castling), true);
                    }
                }

                if (can_castle[static_cast<std::uint8_t>(Color::Black)][1]) // Queen side
                {
                    if (
                            (bqs_clear & all_blockers) == 0 &&
                            (bqs_safe & enemy_threat) == 0
                       )
                    {
                        add_move(movelist, Move(king_square, 7*8+2, MoveSpecial::Castling), true);
                    }
                }
            }
        }
        else // Generate evasive
        {
            std::uint8_t list_size = movelist.size();
            movelist.clear();

            for (std::uint8_t i = 0; i < list_size; i++)
            {
                const Move& move = movelist.at(i);
                const Square from_sq = move.get_from();
                const Square to_sq = move.get_to();
                const std::uint8_t fx = from_sq%8;
                const std::uint8_t fy = from_sq/8;
                const std::uint8_t tx = to_sq%8;
                const Tile tile = get_tile(from_sq);

                if (tile.piece == Piece::King)
                {
                    if (!bitboard_read(enemy_threat, to_sq))
                    {
                        add_move(movelist, Move(king_square, to_sq, MoveSpecial::None), true);
                    }
                }
                else if (bitboard_count(checkers) != 2)
                {
                    if (bitboard_read(pinned, from_sq))
                            continue;

                    if (
                            bitboard_read(checkers | check_blockers, to_sq) &&
                            !bitboard_read(pinned, from_sq)
                       )
                    {
                        add_move(movelist, move, true);
                    }

                    const Square chk = bitboard_bitscan_forward(checkers);
                    const std::uint8_t chk_x = chk%8;
                    const std::uint8_t chk_y = chk/8;

                    if (
                            ep_x != 9 &&
                            tile.piece == Piece::Pawn &&
                            fx != tx &&
                            get_piece(chk_x, chk_y) == Piece::Pawn &&
                            fy == chk_y &&
                            tx == chk_x
                       )
                    {
                        add_move(movelist, Move(from_sq, to_sq, MoveSpecial::EnPassant), true);
                    }
                }
            }
        }

        return;
    }

    void static_analysis() const
    {
        if (static_found)
            return;

        Color their_color = Color::White;
        if (turn == Color::White)
            their_color = Color::Black;

        const Bitboard all_blockers = ~colors[static_cast<std::uint8_t>(Color::Empty)];

        const std::array<Square, 2> king_squares =
        {
            bitboard_bitscan_forward(get_bitboard(turn, Piece::King)),
            bitboard_bitscan_forward(get_bitboard(their_color, Piece::King))
        };

        Bitboard it_pieces = all_blockers;
        while (it_pieces)
        {
            const Square from_square = bitboard_bitscan_forward_pop(it_pieces);
            const std::uint8_t from_x = from_square%8;
            const std::uint8_t from_y = from_square/8;

            const Tile tile = get_tile(from_x, from_y);

            Color them_color = Color::White;
            if (tile.color == Color::White)
                them_color = Color::Black;

            Square their_king_square = king_squares[0];
            if (tile.color == turn)
                their_king_square = king_squares[1];

            Bitboard attacks = 0;

            switch (tile.piece)
            {
                case Piece::Pawn:
                    {
                        if (tile.color == Color::White)
                        {
                            attacks = movegen_rays[static_cast<std::uint8_t>(Ray::WhitePawnAttacks)][from_square];
                        }
                        else
                        {
                            attacks = movegen_rays[static_cast<std::uint8_t>(Ray::BlackPawnAttacks)][from_square];
                        }

                        if (tile.color == turn)
                        {
                            Bitboard target = colors[static_cast<std::uint8_t>(their_color)];

                            if (tile.color == turn && (ep_x != 9))
                            {
                                if (turn == Color::White)
                                    bitboard_set(target, ep_x, 5);
                                else
                                    bitboard_set(target, ep_x, 2);
                            }

                            attacks &= target;
                        }
                    }
                    break;

                case Piece::Knight:
                    {
                        attacks = movegen_rays[static_cast<std::uint8_t>(Ray::Knight)][from_square];
                    }
                    break;

                case Piece::Bishop:
                    {
                        for (std::uint8_t d = 0; d < 8; d+=2)
                        {
                            attacks |= movegen_rays[d][from_square];

                            Bitboard blockers = movegen_rays[d][from_square] & all_blockers;

                            if (blockers != 0)
                            {
                                Square blocker_square = bitboard_bitscan(blockers, d);
                                attacks &= ~movegen_rays[d][blocker_square];
                            }

                            if (bitboard_read(blockers, their_king_square))
                            {
                                Bitboard between =
                                    movegen_rays[d][from_square] &
                                    (~movegen_rays[d][their_king_square]);
                                bitboard_unset(between, their_king_square);

                                if (
                                        bitboard_count(between & (colors[static_cast<std::uint8_t>(them_color)])) == 1 &&
                                        bitboard_count(between & (colors[static_cast<std::uint8_t>(tile.color)])) == 0
                                   )
                                {
                                    Square blocker_square = bitboard_bitscan(blockers, d);
                                    bitboard_set(pinned, blocker_square);
                                }

                                if (bitboard_count(blockers & between) == 0)
                                    check_blockers |= between;

                                if (tile.color == their_color)
                                {
                                    // Extend threat beyond king
                                    bitboard_unset(blockers, their_king_square);
                                    if (blockers != 0)
                                    {
                                        Square blocker2 = bitboard_bitscan(blockers, d);

                                        enemy_threat |= movegen_rays[d][from_square] & (~movegen_rays[d][blocker2]);
                                    }
                                    else
                                    {
                                        enemy_threat |= movegen_rays[d][from_square];
                                    }
                                }
                            }
                        }
                    }
                    break;

                case Piece::Rook:
                    {
                        for (std::uint8_t d = 1; d < 8; d+=2)
                        {
                            attacks |= movegen_rays[d][from_square];

                            Bitboard blockers = movegen_rays[d][from_square] & all_blockers;

                            if (blockers != 0)
                            {
                                Square blocker_square = bitboard_bitscan(blockers, d);
                                attacks &= ~movegen_rays[d][blocker_square];
                            }

                            if (bitboard_read(blockers, their_king_square))
                            {
                                Bitboard between =
                                    movegen_rays[d][from_square] &
                                    (~movegen_rays[d][their_king_square]);
                                bitboard_unset(between, their_king_square);

                                if (
                                        bitboard_count(between & (colors[static_cast<std::uint8_t>(them_color)])) == 1 &&
                                        bitboard_count(between & (colors[static_cast<std::uint8_t>(tile.color)])) == 0
                                   )
                                {
                                    Square blocker_square = bitboard_bitscan(blockers, d);
                                    bitboard_set(pinned, blocker_square);
                                }

                                if (bitboard_count(blockers & between) == 0)
                                    check_blockers |= between;

                                if (tile.color == their_color)
                                {
                                    // Extend threat beyond king
                                    bitboard_unset(blockers, their_king_square);
                                    if (blockers != 0)
                                    {
                                        Square blocker2 = bitboard_bitscan(blockers, d);

                                        enemy_threat |= movegen_rays[d][from_square] & (~movegen_rays[d][blocker2]);
                                    }
                                    else
                                    {
                                        enemy_threat |= movegen_rays[d][from_square];
                                    }
                                }
                            }
                        }
                    }
                    break;

                case Piece::Queen:
                    {
                        for (std::uint8_t d = 0; d < 8; d++)
                        {
                            attacks |= movegen_rays[d][from_square];

                            Bitboard blockers = movegen_rays[d][from_square] & all_blockers;

                            if (blockers != 0)
                            {
                                Square blocker_square = bitboard_bitscan(blockers, d);
                                attacks &= ~movegen_rays[d][blocker_square];
                            }

                            if (bitboard_read(blockers, their_king_square))
                            {
                                Bitboard between =
                                    movegen_rays[d][from_square] &
                                    (~movegen_rays[d][their_king_square]);
                                bitboard_unset(between, their_king_square);

                                if (
                                        bitboard_count(between & (colors[static_cast<std::uint8_t>(them_color)])) == 1 &&
                                        bitboard_count(between & (colors[static_cast<std::uint8_t>(tile.color)])) == 0
                                   )
                                {
                                    Square blocker_square = bitboard_bitscan(blockers, d);
                                    bitboard_set(pinned, blocker_square);
                                }

                                if (bitboard_count(blockers & between) == 0)
                                    check_blockers |= between;

                                if (tile.color == their_color)
                                {
                                    // Extend threat beyond king
                                    bitboard_unset(blockers, their_king_square);
                                    if (blockers != 0)
                                    {
                                        Square blocker2 = bitboard_bitscan(blockers, d);

                                        enemy_threat |= movegen_rays[d][from_square] & (~movegen_rays[d][blocker2]);
                                    }
                                    else
                                    {
                                        enemy_threat |= movegen_rays[d][from_square];
                                    }
                                }
                            }
                        }
                    }
                    break;

                default:
                    break;
            }

            if (tile.color == turn)
            {
                threat |= attacks;
            }
            else
            {
                enemy_threat |= attacks;

                if (bitboard_read(attacks, king_squares[0]))
                {
                    bitboard_set(checkers, from_square);
                }
            }
        }

        // King threat
        std::array<Bitboard, 2> king_threats =
        {
            movegen_rays[static_cast<std::uint8_t>(Ray::King)][king_squares[0]],
            movegen_rays[static_cast<std::uint8_t>(Ray::King)][king_squares[1]]
        };

        enemy_threat |= king_threats[1];

        bitboard_unset(check_blockers, king_squares[0]);

        if (bitboard_count(checkers) > 1)
        {
            check_blockers = 0;
        }

        static_found = true;
        return;
    }

    void add_moves(MoveList& list, Square from_sq, Bitboard b, bool dont_promote = false) const
    {
        while (b)
        {
            Square to_sq = bitboard_bitscan_forward_pop(b);

            add_move(list, Move(from_sq, to_sq, MoveSpecial::None), dont_promote);
        }
    }

    void add_move(MoveList& list, Move m, bool dont_promote = false) const
    {
        const Tile tile = get_tile(m.get_from());
        const std::uint8_t ty = m.get_to()/8;

        if (tile.piece == Piece::Pawn && ((ty == 0) || (ty == 7)))
        {
            m.set_type(MoveSpecial::Promotion);

            if (dont_promote)
            {
                list.add_move(m);
            }
            else
            {
                m.set_promo(Piece::Knight);
                list.add_move(m);
                m.set_promo(Piece::Bishop);
                list.add_move(m);
                m.set_promo(Piece::Rook);
                list.add_move(m);
                m.set_promo(Piece::Queen);
                list.add_move(m);
            }
        }
        else
        {
            list.add_move(m);
        }
    }

    // Board state
    std::array<Bitboard, 3> colors = {0};
    std::array<Bitboard, 6> pieces = {0};
    Color turn = Color::White;
    std::array<std::array<bool, 2>, 2> can_castle; // KQkq
    std::uint8_t ep_x = 9; // x value for en passant, 9 if no en passant
    std::uint8_t repeatable_movecount = 0;
    std::uint16_t turn_number = 0;

    // Zobrist
    mutable std::uint64_t zobrist_hash = 0;

    // Static analysis
    mutable bool static_found = false;
    mutable Bitboard threat = 0;
    mutable Bitboard enemy_threat = 0;
    mutable Bitboard checkers = 0;
    mutable Bitboard check_blockers = 0;
    mutable Bitboard pinned = 0;

public:
    // Move to get here
    Move movetohere;
    MoveType typetohere = MoveType::Quiet;
};

#endif
