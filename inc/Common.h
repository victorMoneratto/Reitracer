#pragma once

#include <cinttypes>
#include <cstdlib>

using b32 = int32_t;
using i32 = int32_t;
using i64 = int64_t;
using f32 = float;

using u8 = uint8_t;
using u32 = uint32_t;

const f32 PI = 3.14159265358979323846f;

const f32 DEG2RAD = PI/180.f;
const f32 RAD2DEG = 1.f/DEG2RAD;


f32 RandFloat01() {
	return (rand() % (RAND_MAX-1))/ f32(RAND_MAX); // REPLACE THIS
}

f32 Schlick(f32 cos, f32 ri) {
	f32 r0 = (1-ri) / (1+ri);
	r0 = r0 * r0;
	return r0 + (1-r0) * powf(1-cos, 5);
}