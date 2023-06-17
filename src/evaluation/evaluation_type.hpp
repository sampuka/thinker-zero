#ifndef EVALUATION_EVALUATION_TYPE_HPP
#define EVALUATION_EVALUATION_TYPE_HPP

enum class EVALUATION_TYPE
{
	NONE,                            // Used for default.
	JUST_MATERIAL,                   // This is the classic standard material count. Queen = 9.
	SIMPLIFIED_EVALUATION_FUNCTION,  // https://www.chessprogramming.org/Simplified_Evaluation_Function
};

#endif  // EVALUATION_EVALUATION_TYPE_HPP
