#include "Search.hpp"

#include "evaluation/evaluation.hpp"
#include "position/PositionString.hpp"

#include <limits>
#include <math.h>

Search::Search() : m_evaluation_type(EVALUATION_TYPE::NONE)
{
}

Search::Search(EVALUATION_TYPE evaluation_type) : m_evaluation_type(evaluation_type)
{
}

void Search::set_evaluation_type(EVALUATION_TYPE evaluation_type)
{
	m_evaluation_type = evaluation_type;
}

std::vector<unsigned int> Search::search_for_best_move(const Position& position, const MoveList& legal_moves, const unsigned int search_depth)
{
	if (m_evaluation_type == EVALUATION_TYPE::NONE)
	{
		return std::vector<unsigned int>();
	}

	std::vector<int> evaluations;
	std::cout << "Evaluation: ";
	for (const Move& move : legal_moves)
	{
		Position temporary_position = position;
		temporary_position.make_move(move);
		evaluations.push_back(minimaxi(temporary_position, -std::numeric_limits<int>::max(), std::numeric_limits<int>::max(), search_depth - 1));

		std::cout << evaluations.back() << " (" << move.get_string() << ")"
				  << " ";
	}
	std::cout << std::endl;
	return find_index_with_best_evaluation(evaluations, position.get_player());
}

int Search::minimaxi(const Position& position, int alpha, int beta, unsigned int depth)
{
	// If we are at our max search depth then evaluate position and return it.
	if (depth == 0)
	{
		return evaluate_board(position, m_evaluation_type);
	}

	if (position.get_player() == Color::White)  // player is white
	{
		int max_evaluation = -std::numeric_limits<int>::max();
		const MoveList current_legal_moves = generate_legal_moves(position);
		for (const Move& move : current_legal_moves)
		{
			Position temporary_position = position;
			temporary_position.make_move(move);
			int evaluation = minimaxi(temporary_position, alpha, beta, depth - 1);
			max_evaluation = std::max(max_evaluation, evaluation);
			alpha = std::max(alpha, evaluation);
			if (beta <= alpha)
			{
				break;
			}
		}
		return max_evaluation;
	}
	else  // player is black
	{
		int min_evaluation = std::numeric_limits<int>::max();
		const MoveList current_legal_moves = generate_legal_moves(position);
		for (const Move& move : current_legal_moves)
		{
			Position temporary_position = position;
			temporary_position.make_move(move);
			int evaluation = minimaxi(temporary_position, alpha, beta, depth - 1);
			min_evaluation = std::min(min_evaluation, evaluation);
			beta = std::min(beta, evaluation);
			if (beta <= alpha)
			{
				break;
			}
		}
		return min_evaluation;
	}
}

// This can be done better. A quick fix for now.
std::vector<unsigned int> Search::find_index_with_best_evaluation(const std::vector<int>& evaluations, const Color& player)
{
	int best_evaluation = (player == Color::White) ? -std::numeric_limits<int>::max() : std::numeric_limits<int>::max();

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