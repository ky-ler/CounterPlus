#include "entities.h"
#include <vector>
#include "../utils/globals.h"

void entities::GetEntities() {
	entity_list.reserve(32);

	while (globals::is_running) {
		local_player.SetAddress(memory::client + offsets::signatures::dwLocalPlayer);

		for (int i = 1; i <= 32; i++) {
			Entity entity = Entity{ memory::client + offsets::signatures::dwEntityList + (0x10 * i) };

			if (!entity.IsValid()) {
				continue;
			}

			if (entity.IsDormant() || entity.Health() < 1) {
				continue;
			}

			const auto result = std::find_if(
				entity_list.begin(), 
				entity_list.end(),
				[&entity](const Entity& ent) noexcept -> bool { return ent == entity; }
			);

			if (result != std::end(entity_list)) {
				continue;
			}

			entity_list.emplace_back(entity);
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(25));
	}
}