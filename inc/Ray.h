#pragma once

#include <Vec3.h>

struct Ray {
	Ray() = default;

	explicit Ray(const vec3& origin, const vec3& dir)
		: origin(origin), dir(dir) {}

	vec3 PointAt(f32 t) const { return origin + t * dir; }

	vec3 origin;
	vec3 dir;
};