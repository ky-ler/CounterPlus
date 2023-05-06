#pragma once
#include <Windows.h>
#include "../utils/vector.h"
#include "../entities/entities.h"

namespace features {
	static const LPCSTR window_title = "Counter-Strike: Global Offensive - Direct3D 9";

	namespace triggerbot {
		void Triggerbot();
		void Loop();
	}

	namespace esp {
		void DrawEsp();
		bool WorldToScreen(const Vector3& world, Vector3& screen, const ViewMatrix& vm);
	}

	namespace bhop {
		void Bunnyhop();
		void Loop();
	}
}