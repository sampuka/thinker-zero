#ifndef ENGINE_SETTINGS_HPP
#define ENGINE_SETTINGS_HPP

#include "engine/UCISetting.hpp"

class Settings
{
public:
	Settings() = default;

	std::string get_uci_string() const;

	SettingID find_id_by_name(const std::string& name) const;

	// Setting accessors
	bool get_random_moves_only() const;
	void set_random_moves_only(bool value);

private:
	// Settings
	bool random_moves_only = false;
};

inline Settings engine_settings;

#endif  // ENGINE_SETTINGS_HPP
