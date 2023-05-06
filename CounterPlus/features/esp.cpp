#include "features.h"
#include "../imgui/imgui.h"
#include "../utils/globals.h"

bool features::esp::WorldToScreen(const Vector3& world, Vector3& screen, const ViewMatrix& vm) {
	float w = vm[3][0] * world.x + vm[3][1] * world.y + vm[3][2] * world.z + vm[3][3];

	if (w < 0.001f) {
		return false;
	}

	const float x = world.x * vm[0][0] + world.y * vm[0][1] + world.z * vm[0][2] + vm[0][3];
	const float y = world.x * vm[1][0] + world.y * vm[1][1] + world.z * vm[1][2] + vm[1][3];

	w = 1.f / w;
	float nx = x * w;
	float ny = y * w;

	const ImVec2 size = ImGui::GetIO().DisplaySize;

	screen.x = (size.x * 0.5f * nx) + (nx + size.x * 0.5f);
	screen.y = -(size.y * 0.5f * ny) + (ny + size.y * 0.5f);

	return true;
}

void features::esp::DrawEsp() {
	if (globals::esp::enable) {
		const auto local_player_team = entities::local_player.Team();
		const auto view_matrix = memory::Read<ViewMatrix>(memory::client + offsets::signatures::dwViewMatrix);

		for (uint32_t i = 1; i <= 32; i++) {
			Entity entity = Entity{ memory::client + offsets::signatures::dwEntityList + (0x10 * i) };

			if (!entity.IsValid()) 
				continue;

			if (entity.IsDormant()) 
				continue;

			if (!entity.IsAlive()) 
				continue;

			const auto bones = memory::Read<DWORD>(entity.base_address + offsets::netvars::m_dwBoneMatrix);

			if (!bones) {
				continue;
			}

			Vector3 head_pos{
				memory::Read<float>(bones + 0x30 * 8 + 0x0C),
				memory::Read<float>(bones + 0x30 * 8 + 0x1C),
				memory::Read<float>(bones + 0x30 * 8 + 0x2C)
			};

			auto feet_pos = memory::Read<Vector3>(entity.base_address + offsets::netvars::m_vecOrigin);

			Vector3 top{};
			Vector3 bottom{};

			if (WorldToScreen(head_pos + Vector3{ 0, 0, 11.f }, top, view_matrix) && WorldToScreen(feet_pos - Vector3{ 0, 0, 7.f }, bottom, view_matrix)) {
				const float h = bottom.y - top.y;
				const float w = h * 0.35f;

				if (entity.Team() == local_player_team) {
					ImGui::GetBackgroundDrawList()->AddRect(
						{ top.x - w, top.y },
						{ top.x + w, bottom.y },
						ImColor(globals::esp::team_color[0], globals::esp::team_color[1], globals::esp::team_color[2])
					);
				}
				else {
					ImGui::GetBackgroundDrawList()->AddRect(
						{ top.x - w, top.y }, 
						{ top.x + w, bottom.y },
						ImColor(globals::esp::enemy_color[0], globals::esp::enemy_color[1], globals::esp::enemy_color[2])
					);
				}
			}
		}
		//std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}
}