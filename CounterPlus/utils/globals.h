#pragma once

namespace globals {
	inline bool is_running = true;
	inline bool show_menu = true;

	namespace triggerbot {
		inline bool enable = true;
		inline bool humanizer = false;
		inline int pre_delay = 0;
		inline int hold_time = 50;
		inline int post_delay = 250;
		inline int humanize_min = 5;
		inline int humanize_max = 30;
	}

	namespace esp {
		inline bool enable = true;
		inline float enemy_color[3] = { 1.f, 0.f, 0.f };
		inline float team_color[3] = { 0.f, 0.f, 1.f };
	}

	namespace bhop {
		inline bool enable = true;
	}
}