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
	MaxSearchDepth,
	LogFilepath
};

class UCISetting
{
public:
	UCISetting() = delete;

	// Check constructor
	constexpr UCISetting(SettingID id, const char* name, bool check_default) : m_id(id), m_type(UCISettingType::Check), m_name(name), m_check_default(check_default)
	{
	}

	// Spin constructor
	constexpr UCISetting(SettingID id, const char* name, int32_t spin_default, int32_t spin_min, int32_t spin_max)
		: m_id(id), m_type(UCISettingType::Spin), m_name(name), m_spin_default(spin_default), m_spin_min(spin_min), m_spin_max(spin_max)
	{
	}

	// Combo constructor
	constexpr UCISetting(SettingID id, const char* name, const char* combo_default, std::vector<std::string> combo_values)
		: m_id(id), m_type(UCISettingType::Combo), m_name(name), m_combo_default(combo_default), m_combo_values(combo_values)
	{
	}

	// String constructor
	constexpr UCISetting(SettingID id, const char* name, const char* string_default) : m_id(id), m_type(UCISettingType::String), m_name(name), m_string_default(string_default)
	{
	}

	std::string get_print_string()
	{
		std::string string;

		string += "option name " + m_name + " type ";

		switch (m_type)
		{
			case UCISettingType::Check:
			{
				string += "check default ";

				if (m_check_default)
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
				string += "spin default " + std::to_string(m_spin_default) + " min " + std::to_string(m_spin_min) + " max " + std::to_string(m_spin_max);
				break;
			}

			case UCISettingType::Combo:
			{
				string += "combo default " + m_combo_default;

				for (const std::string& var : m_combo_values)
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
				string += "string default " + m_string_default;
				break;
			}
		}

		return string;
	}

	SettingID get_id() const
	{
		return m_id;
	}

	bool match(const std::string& other_name) const
	{
		return string_compare(m_name, other_name);
	}

private:
	const SettingID m_id;
	const UCISettingType m_type;
	const std::string m_name;

	// If Check type
	bool m_check_default = false;

	// If Spin type
	int32_t m_spin_default = 0;
	int32_t m_spin_min = 0;
	int32_t m_spin_max = 0;

	// If Combo type
	std::string m_combo_default = "<empty>";
	std::vector<std::string> m_combo_values = {};

	// If String type
	std::string m_string_default = "<empty>";

	// No state for Button type
};

#endif  // ENGINE_UCISETTING_HPP
