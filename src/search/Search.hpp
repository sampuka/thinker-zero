#ifndef SEARCH_SEARCH_HPP
#define SEARCH_SEARCH_HPP

#include "movegen/movegen.hpp"
#include "position/Position.hpp"

#include <iostream>
#include <limits>
#include <vector>

class Search
{
public:  // Methods.
	Search();
	~Search() = default;

	std::vector<unsigned int> search_for_best_move(const Position& position, const MoveList& legal_moves, const unsigned int search_depth);

private:  // Methods.
	float minimaxi(const Position& position, float alpha, float beta, unsigned int depth);

	// Find index with best evaluation for that player. If more evaluation is equal, we chose random.
	std::vector<unsigned int> find_index_with_best_evaluation(const std::vector<float>& evaluations, const Color& player);

	// Dumme method for now.
	float max_float(const float f1, const float f2);
	float min_float(const float f1, const float f2);
};

#endif  // SEARCH_SEARCH_HPP
