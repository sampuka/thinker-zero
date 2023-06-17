#include "PositionAnalysis.hpp"

#include "movegen/movegen.hpp"

PositionAnalysis::PositionAnalysis(const Position& position) : m_position(position)
{
}

bool PositionAnalysis::king_in_check() const
{
	Color enemy_color = get_other_color(m_position.get_player());

	MoveList pseudolegal_moves = generate_pseudolegal_moves(m_position);

	for (uint8_t i = 0; i < 64; i++)
	{
		Square square(i);

		if (m_position.get_color(square) == enemy_color && m_position.get_piece(square) == Piece::King)
		{
			for (const Move& move : pseudolegal_moves)
			{
				if (move.get_to_square().get_data() == square.get_data())
				{
					return true;
				}
			}

			return false;
		}
	}

	return false;
}
