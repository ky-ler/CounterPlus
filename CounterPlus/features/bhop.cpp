#include "features.h"
#include "../utils/globals.h"

void features::bhop::Bunnyhop() {

	if (entities::local_player.Flags() & (1 << 0)) {
		memory::Write<int32_t>(memory::client + offsets::signatures::dwForceJump, 5);
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
		memory::Write<int32_t>(memory::client + offsets::signatures::dwForceJump, 4);
	}
}

void features::bhop::Loop() {
	while (true) {
		if (globals::bhop::enable) {
			if (GetAsyncKeyState(VK_SPACE) && GetForegroundWindow() == FindWindowA(nullptr, window_title)){
				Bunnyhop();
			}
		}
	}
}