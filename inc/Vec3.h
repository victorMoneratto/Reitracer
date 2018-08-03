#pragma once

#include <common.h>
#include <iostream>
#include <cmath>
#include <Common.h>
#include <algorithm>

struct vec3 {
	explicit vec3() = default;
	explicit vec3(f32 vX) : v { vX, vX, vX } {}
	explicit vec3(f32 v0, f32 v1, f32 v2) : v{ v0, v1, v2 } {}
	vec3& operator=(const vec3& rhs) = default;

	inline f32 operator[](i32 i) const { return v[i]; }
	inline f32& operator[](i32 i) { return v[i]; }

	inline const vec3 operator+() const { return *this; }
	inline const vec3 operator-() const { return vec3(-v[0], -v[1], -v[2]); }

	inline vec3 operator+=(const vec3& rhs);
	inline vec3 vec3::operator-=(const vec3& rhs);
	inline vec3 vec3::operator*=(const vec3& rhs);
	inline vec3 vec3::operator/=(const vec3& rhs);
	inline vec3 vec3::operator*=(const f32 rhs);
	inline vec3 vec3::operator/=(const f32 rhs);

	inline f32 SqrLength() const;
	inline f32 Length() const;
	inline vec3& Normalize();

	union {
		struct { f32 x, y, z; };
		struct { f32 r, g, b; };
		f32 v[3];
	};
};

//////////////////////////////////////////////////////////////////////////
// vec3 binary operators
inline vec3 operator+(const vec3& lhs, const vec3& rhs) {
	return vec3(lhs[0] + rhs[0], lhs[1] + rhs[1], lhs[2] + rhs[2]);
}

inline vec3 operator-(const vec3& lhs, const vec3& rhs) {
	return vec3(lhs[0] - rhs[0], lhs[1] - rhs[1], lhs[2] - rhs[2]);
}

inline vec3 operator*(const vec3& lhs, const vec3& rhs) {
	return vec3(lhs[0] * rhs[0], lhs[1] * rhs[1], lhs[2] * rhs[2]);
}

inline vec3 operator/(const vec3& lhs, const vec3& rhs) {
	return vec3(lhs[0] / rhs[0], lhs[1] / rhs[1], lhs[2] / rhs[2]);
}

inline vec3 operator*(f32 lhs, const vec3& rhs) {
	return vec3(lhs * rhs[0], lhs * rhs[1], lhs * rhs[2]);
}

inline vec3 operator*(const vec3& lhs, f32 rhs) {
	return vec3(rhs * lhs[0], rhs * lhs[1], rhs * lhs[2]);
}

inline vec3 operator/(const vec3& lhs, f32 rhs) {
	return vec3(lhs[0] / rhs, lhs[1] / rhs, lhs[2] / rhs);
}

//////////////////////////////////////////////////////////////////////////
// compound assignment operators
inline vec3 vec3::operator+=(const vec3& rhs) { *this = *this + rhs; return *this; }
inline vec3 vec3::operator-=(const vec3& rhs) { *this = *this - rhs; return *this; }
inline vec3 vec3::operator*=(const vec3& rhs) { *this = *this * rhs; return *this; }
inline vec3 vec3::operator/=(const vec3& rhs) { *this = *this / rhs; return *this; }
inline vec3 vec3::operator*=(const f32 rhs) { *this = *this * rhs; return *this; }
inline vec3 vec3::operator/=(const f32 rhs) { *this = *this / rhs; return *this; }

//////////////////////////////////////////////////////////////////////////
// products & operations
inline f32 Dot(const vec3& lhs, const vec3& rhs) {
	return (lhs[0] * rhs[0] + lhs[1] * rhs[1] + lhs[2] * rhs[2]);
}

inline vec3 Cross(const vec3& lhs, const vec3& rhs) {
	f32 a = lhs.y * rhs.z - lhs.z * rhs.y;
	f32 b = lhs.z * rhs.x - lhs.x * rhs.z;
	f32 c = lhs.x * rhs.y - lhs.y * rhs.x;
	return vec3(a, b, c);
}

inline f32 vec3::SqrLength() const {
	return Dot(*this, *this);
}

inline f32 vec3::Length() const {
	return sqrtf(Dot(*this, *this));
}

inline vec3 Normalized(const vec3& v) {
	return v / v.Length();
}

inline vec3& vec3::Normalize() {
	return *this = Normalized(*this);
}

inline vec3 Lerp(const vec3& a, const vec3& b, const f32 t) {
	return (1 - t) * a + t * b;
}

inline vec3 Reflect(const vec3& v, const vec3& n) {
	return v - 2 * Dot(v, n) * n;
}

inline b32 Refract(const vec3& v, const vec3& n, f32 nint, vec3& outRefracted) {
	vec3 vnorm = Normalized(v);
	f32 dt = Dot(vnorm, n);
	f32 discr = 1.0f - nint * nint * (1 - dt * dt);
	if (discr > 0) {
		outRefracted = nint * (vnorm - n * dt) - n * sqrtf(discr);
		return true;
	}
	return false;
}

//////////////////////////////////////////////////////////////////////////
// io
inline std::ostream& operator<<(std::ostream& os, const vec3& v) {
	os << v.x << " " << v.y << " " << v.z;
	return os;
}

//////////////////////////////////////////////////////////////////////////
// other functions

vec3 RandomInUnitSphere() {
	vec3 p;
	do {
		p = vec3(RandFloat01(), RandFloat01(), RandFloat01());
	} while(p.SqrLength() >= 1);

	return p;
}

vec3 RandomInUnitDisk() {
	vec3 p; 
	do {
		p = 2.f * vec3(RandFloat01(), RandFloat01(), 0) - vec3(1, 1, 0);
	} while (p.SqrLength() >= 1);
	return p;
}

vec3 LinearToSRGB(vec3 rgb) {
	return vec3(std::max(1.055f * powf(rgb.r, 0.416666667f) - 0.055f, 0.0f),
				std::max(1.055f * powf(rgb.g, 0.416666667f) - 0.055f, 0.0f),
				std::max(1.055f * powf(rgb.b, 0.416666667f) - 0.055f, 0.0f));
}