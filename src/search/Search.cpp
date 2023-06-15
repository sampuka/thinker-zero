#include "Search.hpp"

#include "evaluation/evaluation.hpp"
#include "position/PositionString.hpp"

#include <limits>
#include <math.h>

Search::Search()
{
}

std::vector<unsigned int> Search::search_for_best_move(const Position& position, const MoveList& legal_moves, const unsigned int search_depth)
{
	std::vector<float> evaluations;
	for (const Move& move : legal_moves)
	{
		Position temporary_position = position;
		temporary_position.make_move(move);
		evaluations.push_back(minimaxi(temporary_position, -std::numeric_limits<float>::max(), std::numeric_limits<float>::max(), search_depth - 1));
	}
	return find_index_with_best_evaluation(evaluations, position.get_player());
}

float Search::minimaxi(const Position& position, float alpha, float beta, unsigned int depth)
{
	// If we are at our max search depth then evaluate position and return it.
	if (depth == 0)
	{
		return evaluation::evaluate_board(position);
	}

	if (position.get_player() == Color::White)  // player is white
	{
		float max_evaluation = -std::numeric_limits<float>::max();
		const MoveList current_legal_moves = generate_legal_moves(position);
		for (const Move& move : current_legal_moves)
		{
			Position temporary_position = position;
			temporary_position.make_move(move);
			float evaluation = minimaxi(temporary_position, alpha, beta, depth - 1);
			max_evaluation = max_float(max_evaluation, evaluation);
			alpha = max_float(alpha, evaluation);
			if (beta <= alpha)
			{
				break;
			}
		}
		return max_evaluation;
	}
	else  // player is black
	{
		float min_evaluation = std::numeric_limits<float>::max();
		const MoveList current_legal_moves = generate_legal_moves(position);
		for (const Move& move : current_legal_moves)
		{
			Position temporary_position = position;
			temporary_position.make_move(move);
			float evaluation = minimaxi(temporary_position, alpha, beta, depth - 1);
			min_evaluation = min_float(min_evaluation, evaluation);
			beta = min_float(beta, evaluation);
			if (beta <= alpha)
			{
				break;
			}
		}
		return min_evaluation;
	}
}

// This can be done better. A quick fix for now.
std::vector<unsigned int> Search::find_index_with_best_evaluation(const std::vector<float>& evaluations, const Color& player)
{
	float best_evaluation = (player == Color::White) ? -std::numeric_limits<float>::max() : std::numeric_limits<float>::max();

	for (unsigned int i = 0; i < evaluations.size(); i++)
	{
		if (player == Color::White)
		{
			if (evaluations[i] > best_evaluation)
			{
				best_evaluation = evaluations[i];
			}
		}
		else
		{
			if (evaluations[i] < best_evaluation)
			{
				best_evaluation = evaluations[i];
			}
		}
	}

	// Checking if more moves has same evaluation
	std::vector<unsigned int> best_indices;
	std::cout << "best evaluation: " << best_evaluation << std::endl;
	for (unsigned int i = 0; i < evaluations.size(); i++)
	{
		if (evaluations[i] == best_evaluation)
		{
			best_indices.push_back(i);
		}
	}
	std::cout << "Equally good moves: " << best_indices.size() << " - all possible moves: " << evaluations.size() << std::endl;
	return best_indices;
}

float Search::max_float(const float f1, const float f2)
{
	return (f1 > f2) ? f1 : f2;
}

float Search::min_float(const float f1, const float f2)
{
	return (f1 < f2) ? f1 : f2;
}

const std::string Search::print_movelist(const MoveList& moves)
{
	std::string str = "[";
	for (const Move& move : moves)
	{
		str += " " + converting::convert_move_to_string(move);
	}
	str += "]";
	return str;
}
