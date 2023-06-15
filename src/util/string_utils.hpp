#ifndef UTIL_STRING_UTILS_HPP
#define UTIL_STRING_UTILS_HPP

#include <string_view>

#include <cctype>

inline bool string_compare(std::string_view a, std::string_view b)
{
	if (a.length() != b.length())
	{
		return false;
	}

	for (size_t i = 0; i < a.length(); i++)
	{
		if (std::tolower(a[i]) != std::tolower(b[i]))
		{
			return false;
		}
	}

	return true;
}

#endif  // UTIL_STRING_UTILS_HPP
