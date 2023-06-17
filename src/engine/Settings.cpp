#include "Settings.hpp"

#include "engine/UCISetting.hpp"

#include <array>

constexpr UCISetting setting_Hash(SettingID::Hash, "Hash", 1, 1, 128);

constexpr UCISetting setting_RandomMovesOnly(SettingID::RandomMovesOnly, "Random moves", false);

constexpr UCISetting setting_MaxSearchDepth(SettingID::MaxSearchDepth, "Search depth", 4, 1, 6);

constexpr UCISetting setting_Logfile(SettingID::LogFilepath, "Log filepath", "");

constexpr std::array<UCISetting, 4> supported_settings = {setting_Hash, setting_RandomMovesOnly, setting_MaxSearchDepth, setting_Logfile};

std::string Settings::get_uci_string() const
{
	std::string full_string;

	for (UCISetting setting : supported_settings)
	{
		full_string += setting.get_print_string() + '\n';
	}

	return full_string;
}

SettingID Settings::find_id_by_name(const std::string& name) const
{
	for (UCISetting setting : supported_settings)
	{
		if (setting.match(name))
		{
			return setting.get_id();
		}
	}

	return SettingID::Unknown;
}

bool Settings::get_random_moves_only() const
{
	return m_random_moves_only;
}

void Settings::set_random_moves_only(bool value)
{
	m_random_moves_only = value;
}

uint8_t Settings::get_max_search_depth() const
{
	return m_max_search_depth;
}

void Settings::set_max_search_depth(uint8_t depth)
{
	m_max_search_depth = depth;
}