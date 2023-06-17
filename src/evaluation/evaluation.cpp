#include "evaluation.hpp"

#include "evaluation/just_material.hpp"
#include "evaluation/simplified_evaluation_function.hpp"

int evaluate_board(const Position& position, EVALUATION_TYPE evaluation_type)
{
	switch (evaluation_type)
	{
		case EVALUATION_TYPE::JUST_MATERIAL:
			return evaluation_jm::evaluate_board(position);

		case EVALUATION_TYPE::SIMPLIFIED_EVALUATION_FUNCTION:
			return evaluation_sef::evaluate_board(position);

		default:
			return 0;
	}
	return 0;
}