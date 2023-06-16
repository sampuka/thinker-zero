#ifndef ENGINE_UCISETTING_HPP
#define ENGINE_UCISETTING_HPP

#include "util/string_utils.hpp"
#include <type_traits>

#include <cstdint>
#include <string>
#include <vector>

enum class UCISettingType
{
	Check,
	Spin,
	Combo,
	Button,
	String
};

enum class SettingID
{
	Unknown,
	Hash,
	RandomMovesOnly,
	MaxSearchDepth
};

class UCISetting
{
public:
	UCISetting() = delete;

	// Check constructor
	constexpr UCISetting(SettingID id_, const char* name_, bool check_default_) : id(id_), type(UCISettingType::Check), name(name_), check_default(check_default_)
	{
	}

	// Spin constructor
	constexpr UCISetting(SettingID id_, const char* name_, int32_t spin_default_, int32_t spin_min_, int32_t spin_max_)
		: id(id_), type(UCISettingType::Spin), name(name_), spin_default(spin_default_), spin_min(spin_min_), spin_max(spin_max_)
	{
	}

	// Combo constructor
	constexpr UCISetting(SettingID id_, const char* name_, const char* combo_default_, std::vector<std::string> combo_values_)
		: id(id_), type(UCISettingType::Combo), name(name_), combo_default(combo_default_), combo_values(combo_values_)
	{
	}

	// String constructor
	constexpr UCISetting(SettingID id_, const char* name_, const char* string_default_) : id(id_), type(UCISettingType::String), name(name_), string_default(string_default_)
	{
	}

	std::string get_print_string()
	{
		std::string string;

		string += "option name " + name + " type ";

		switch (type)
		{
			case UCISettingType::Check:
			{
				string += "check default ";

				if (check_default)
				{
					string += "true";
				}
				else
				{
					string += "false";
				}
				break;
			}

			case UCISettingType::Spin:
			{
				string += "spin default " + std::to_string(spin_default) + " min " + std::to_string(spin_min) + " max " + std::to_string(spin_max);
				break;
			}

			case UCISettingType::Combo:
			{
				string += "combo default " + combo_default;

				for (const std::string& var : combo_values)
				{
					string += " var " + var;
				}
				break;
			}

			case UCISettingType::Button:
			{
				string += "button";
				break;
			}

			case UCISettingType::String:
			{
				string += "string default " + string_default;
				break;
			}
		}

		return string;
	}

	SettingID get_id() const
	{
		return id;
	}

	bool match(const std::string& other_name) const
	{
		return string_compare(name, other_name);
	}

private:
	const SettingID id;
	const UCISettingType type;
	const std::string name;

	// If Check type
	bool check_default = false;

	// If Spin type
	int32_t spin_default = 0;
	int32_t spin_min = 0;
	int32_t spin_max = 0;

	// If Combo type
	std::string combo_default = "<empty>";
	std::vector<std::string> combo_values = {};

	// If String type
	std::string string_default = "<empty>";

	// No state for Button type
};

#endif  // ENGINE_UCISETTING_HPP
