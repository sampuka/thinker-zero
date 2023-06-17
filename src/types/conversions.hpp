#ifndef TYPES_CONVERSIONS_HPP
#define TYPES_CONVERSIONS_HPP

#include "types/Move.hpp"
#include "types/Piece.hpp"

#include <cctype>

inline char convert_piece_to_char(Piece piece)
{
	switch (piece)
	{
		case Piece::Pawn:
		{
			return 'p';
		}
		case Piece::Knight:
		{
			return 'n';
		}
		case Piece::Bishop:
		{
			return 'b';
		}
		case Piece::Rook:
		{
			return 'r';
		}
		case Piece::Queen:
		{
			return 'q';
		}
		case Piece::King:
		{
			return 'k';
		}
		default:
		{
			return '.';
		}
	}
}

inline Piece convert_char_to_piece(char c_)
{
	char c = static_cast<char>(std::tolower(c_));

	switch (c)
	{
		case 'p':
		{
			return Piece::Pawn;
		}
		case 'n':
		{
			return Piece::Knight;
		}
		case 'b':
		{
			return Piece::Bishop;
		}
		case 'r':
		{
			return Piece::Rook;
		}
		case 'q':
		{
			return Piece::Queen;
		}
		case 'k':
		{
			return Piece::King;
		}
		default:
		{
			return Piece::Empty;
		}
	}
}

inline Piece convert_promo_to_piece(MoveType type)
{
	switch (type)
	{
		case MoveType::KnightPromo:
		{
			return Piece::Knight;
		}
		case MoveType::BishopPromo:
		{
			return Piece::Bishop;
		}
		case MoveType::RookPromo:
		{
			return Piece::Rook;
		}
		case MoveType::QueenPromo:
		{
			return Piece::Queen;
		}
		default:
		{
			return Piece::Empty;
		}
	}
}

inline MoveType convert_piece_to_promo(Piece piece)
{
	switch (piece)
	{
		case Piece::Knight:
		{
			return MoveType::KnightPromo;
		}
		case Piece::Bishop:
		{
			return MoveType::BishopPromo;
		}
		case Piece::Rook:
		{
			return MoveType::RookPromo;
		}
		case Piece::Queen:
		{
			return MoveType::QueenPromo;
		}
		default:
		{
			return MoveType::Quiet;
		}
	}
}

#endif  // TYPES_CONVERSIONS_HPP
