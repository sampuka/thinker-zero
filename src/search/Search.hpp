#ifndef SEARCH_SEARCH_HPP
#define SEARCH_SEARCH_HPP

#include "evaluation/evaluation_type.hpp"
#include "movegen/movegen.hpp"
#include "position/Position.hpp"

#include <iostream>
#include <limits>
#include <vector>

class Search
{
public:  // Methods.
	Search();
	Search(EVALUATION_TYPE evaluation_type);
	~Search() = default;

	void set_evaluation_type(EVALUATION_TYPE evaluation_type);

	std::vector<unsigned int> search_for_best_move(const Position& position, const MoveList& legal_moves, const unsigned int search_depth);

private:  // Methods.
	int minimaxi(const Position& position, int alpha, int beta, unsigned int depth);

	// Find index with best evaluation for that player. If more evaluation is equal, we chose random.
	std::vector<unsigned int> find_index_with_best_evaluation(const std::vector<int>& evaluations, const Color& player);

private:  // Variables.
	EVALUATION_TYPE m_evaluation_type;
};

#endif  // SEARCH_SEARCH_HPP
