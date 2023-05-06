#include <thread>
#include <iostream>

#include "entities/entities.h"
#include "overlay/overlay.h"
#include "features/features.h"

bool Init() {
	memory::GetProcess("csgo.exe");

	if (memory::process_id == 0) {
		return false;
	}

	std::cout << "Found CSGO Process ID: " << memory::process_id << std::endl;

	memory::client = memory::GetModuleAddress("client.dll");
	memory::engine = memory::GetModuleAddress("engine.dll");

	std::cout << "Found client.dll: 0x" << std::hex << memory::client << std::endl;
	std::cout << "Found engine.dll: 0x" << std::hex << memory::engine << std::endl;
	memory::OpenPHandle();

    return true;
}

int main() {
	if (!Init()) {
		return 0;
	}

	overlay::CreateHWindow("CounterPlus Stats");
	overlay::CreateDevice();
	overlay::CreateImGui();
	overlay::CreateRenderTarget();

	std::thread(entities::GetEntities).detach();
	std::thread(features::triggerbot::Loop).detach();
	std::thread(features::bhop::Loop).detach();

	while (globals::is_running) {
		overlay::Render();
		std::this_thread::sleep_for(std::chrono::milliseconds(5));
	}

	overlay::DestroyRenderTarget();
	overlay::DestroyImGui();
	overlay::DestroyDevice();
	overlay::DestroyHWindow();

	return 0;
}

