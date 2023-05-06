#pragma once
#include "entity.h"
#include <vector>

namespace entities {
	inline std::vector<Entity> entity_list;
	inline Entity local_player;

	void GetEntities();
}