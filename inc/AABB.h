#pragma once

#include <Vec3.h>
#include <algorithm>

inline f32 ffmin(f32 a, f32 b) { return (a < b ? a : b); }
inline f32 ffmax(f32 a, f32 b) { return (a > b ? a : b); }

struct AABB {
	explicit AABB() = default;
	explicit AABB(const vec3& min, const vec3& max) : min(min), max(max) {}

	b32 hit(const Ray& ray, f32 tmin, f32 tmax) const {
		for (i32 i = 0; i < 3; i++) {
			const f32 invD = 1.f / ray.dir[i];
			f32 t0 = (min[i] - ray.origin[i]) * invD; // first ray-plane intersect
			f32 t1 = (max[i] - ray.origin[i]) * invD; // second ray-plane intersect

			if (invD < 0) {
				 // negative dir hits max first, so swap them 
				 // to correctly compare against (tmin,tmax)
				std::swap(t0, t1);
			}

			tmin = ffmax(tmin, t0);
			tmax = ffmin(tmax, t1);

			if (tmax <= tmin) {
				return false;
			}
		}

		return true;
	}

	vec3 min, max;
};