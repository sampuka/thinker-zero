#include "Settings.hpp"

#include "engine/UCISetting.hpp"

#include <array>

constexpr UCISetting setting_Hash(SettingID::Hash, "Hash", 1, 1, 128);

constexpr UCISetting setting_RandomMovesOnly(SettingID::RandomMovesOnly, "Random Moves", false);

constexpr UCISetting setting_MaxSearchDepth(SettingID::MaxSearchDepth, "Search Depth", 4, 1, 6);

constexpr std::array<UCISetting, 3> supported_settings = {setting_Hash, setting_RandomMovesOnly, setting_MaxSearchDepth};

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
	return random_moves_only;
}

void Settings::set_random_moves_only(bool value)
{
	random_moves_only = value;
}

uint8_t Settings::get_max_search_depth() const
{
	return max_search_depth;
}

void Settings::set_max_search_depth(uint8_t depth)
{
	max_search_depth = depth;
}