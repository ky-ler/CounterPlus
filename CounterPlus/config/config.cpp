#include "config.h"
#include <fstream>
#include "../utils/globals.h"

void config::LoadConfig() {
	json j;

	std::ifstream f("config.json", std::ifstream::binary);

	// use default values if config file not found
	if (f.fail()) { 
		return; 
	} 

	f >> j;

	globals::triggerbot::enable = j.get<ConfigPreset>().EnableTriggerbot;
	globals::triggerbot::humanizer = j.get<ConfigPreset>().EnableHumanizer;
	globals::triggerbot::humanize_min = j.get<ConfigPreset>().HumanizerMin;
	globals::triggerbot::humanize_max = j.get<ConfigPreset>().HumanizerMax;
	globals::triggerbot::pre_delay = j.get<ConfigPreset>().DelayBeforeShot;
	globals::triggerbot::hold_time = j.get<ConfigPreset>().TriggerHoldTime;
	globals::triggerbot::post_delay = j.get<ConfigPreset>().DelayAfterShot;

	globals::esp::enable = j.get<ConfigPreset>().EnableESP;
	globals::esp::enemy_color[0] = j.get<ConfigPreset>().EnemyColorR;
	globals::esp::enemy_color[1] = j.get<ConfigPreset>().EnemyColorG;
	globals::esp::enemy_color[2] = j.get<ConfigPreset>().EnemyColorB;
	globals::esp::team_color[0] = j.get<ConfigPreset>().TeamColorR;
	globals::esp::team_color[1] = j.get<ConfigPreset>().TeamColorG;
	globals::esp::team_color[2] = j.get<ConfigPreset>().TeamColorB;

	globals::bhop::enable = j.get<ConfigPreset>().EnableBunnyhop;
}

void config::SaveConfig(ConfigPreset preset) {
	const json j = preset;
	std::ofstream o("config.json");
	o << std::setw(4) << j << std::endl;
}