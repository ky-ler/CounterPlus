#pragma once
#include <string>
#include "json.hpp"

using json = nlohmann::json;

namespace config {
	struct ConfigPreset {
		bool EnableTriggerbot = true;
		bool EnableHumanizer = true;
		int HumanizerMin = 5;
		int HumanizerMax = 35;
		int DelayBeforeShot = 20;
		int TriggerHoldTime = 50;
		int DelayAfterShot = 250;

		bool EnableESP = true;
		float EnemyColorR = { 0.f };
		float EnemyColorG = { 0.f };
		float EnemyColorB = { 0.f };
		float TeamColorR = { 0.f };
		float TeamColorG = { 0.f };
		float TeamColorB = { 0.f };

		bool EnableBunnyhop = true;
	};

	NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(
		ConfigPreset,
		EnableTriggerbot,
		EnableHumanizer,
		HumanizerMin,
		HumanizerMax,
		DelayBeforeShot,
		TriggerHoldTime,
		DelayAfterShot,
		EnableESP,
		EnemyColorR,
		EnemyColorG,
		EnemyColorB,
		TeamColorR,
		TeamColorG,
		TeamColorB,
		EnableBunnyhop
	)

	void LoadConfig();
	void SaveConfig(ConfigPreset preset);
}