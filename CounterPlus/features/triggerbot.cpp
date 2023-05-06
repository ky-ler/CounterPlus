#include "features.h"
#include <vector>
#include <iostream>
#include "../utils/globals.h"

void features::triggerbot::Loop() {
	while (true) {
		if (globals::triggerbot::enable) {
			if (GetAsyncKeyState(VK_XBUTTON2) && GetForegroundWindow() == FindWindowA(nullptr, window_title)) {
				Triggerbot();
			}
		}
	}
}

void features::triggerbot::Triggerbot() {
	int32_t index = entities::local_player.CrosshairId();

	if (index <= 0 || index >= 64) { 
		return; 
	}

	Entity ent = Entity{ memory::client + offsets::signatures::dwEntityList + (0x10 * (index - 1)) };

	if (ent.Team() == entities::local_player.Team()) {
		return;
	}

	int32_t humanize = 0;

	if (globals::triggerbot::humanizer) {
		humanize = rand() % globals::triggerbot::humanize_max + globals::triggerbot::humanize_min;
	}

	// Wait before firing
	std::this_thread::sleep_for(std::chrono::milliseconds(
		globals::triggerbot::pre_delay +
		globals::triggerbot::humanize_max)
	);

	mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);

	// Hold fire button
	std::this_thread::sleep_for(std::chrono::milliseconds(
		globals::triggerbot::hold_time +
		globals::triggerbot::humanize_max)
	);

	mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);

	// Wait before next shot
	std::this_thread::sleep_for(std::chrono::milliseconds(
		globals::triggerbot::post_delay +
		globals::triggerbot::humanize_max)
	);
}