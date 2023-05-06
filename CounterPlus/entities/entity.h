#pragma once
#include <thread>
#include "../utils/offsets.hpp"
#include "../memory/memory.h"
#include "../utils/vector.h"

class Entity {
public:
	uintptr_t base_address;

	Entity() = default;

	Entity(const uintptr_t& address) :
		base_address(memory::Read<uintptr_t>(address)) { }

	void SetAddress(const uintptr_t& address) {
		base_address = memory::Read<uintptr_t>(address);
	}

	constexpr bool IsValid() {
		return base_address != 0;
	}

	constexpr bool operator==(const Entity& other_entity) {
		return base_address == other_entity.base_address;
	}

	constexpr int32_t Flags() {
		return memory::Read<int32_t>(base_address + offsets::netvars::m_fFlags);
	}

	constexpr int32_t Health() {
		return memory::Read<int32_t>(base_address + offsets::netvars::m_iHealth);
	}

	constexpr int32_t Team() {
		return memory::Read<int32_t>(base_address + offsets::netvars::m_iTeamNum);
	}

	constexpr bool IsAlive() {
		return memory::Read<bool>(base_address + offsets::netvars::m_lifeState) == 0;
	}

	constexpr bool IsDormant() {
		return memory::Read<bool>(base_address + offsets::signatures::m_bDormant);
	}

	constexpr int32_t CrosshairId() {
		return memory::Read<int32_t>(base_address + offsets::netvars::m_iCrosshairId);
	}

	constexpr uint32_t BoneMatrix() {
		return memory::Read<uint32_t>(base_address + offsets::netvars::m_dwBoneMatrix);
	}
};

