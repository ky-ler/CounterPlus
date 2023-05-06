#pragma once
#include <cfloat>

struct Vector3 {
	Vector3() noexcept
		: x(), y(), z() {}

	Vector3(float x, float y, float z) noexcept
		: x(x), y(y), z(z) {}

	Vector3& operator+(const Vector3& v) noexcept {
		x += v.x;
		y += v.y;
		z += v.z;
		return *this;
	}

	Vector3& operator-(const Vector3& v) noexcept {
		x -= v.x;
		y -= v.y;
		z -= v.z;
		return *this;
	}

	float x, y, z;
};

struct ViewMatrix {
	ViewMatrix() noexcept
		: data() {}

	float* operator[](int idx) noexcept {
		return data[idx];
	}

	const float* operator[](int idx) const noexcept {
		return data[idx];
	}

	float data[4][4];
};