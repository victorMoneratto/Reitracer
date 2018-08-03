#pragma once
#include <Ray.h>

struct Hit {
	vec3 pos;
	vec3 normal;
	f32 t;
};

struct Sphere {
	vec3 center;
	f32 radius;
};

struct Material {
	enum Type { Lambert, Metal, Dielectric };
	Type type;
	vec3 albedo;
	f32 roughness;
	f32 ri;
};

b32 HitSphere(const Ray& r, const Sphere& s, f32 tmin, f32 tmax, Hit& outHit) {
	vec3 oc = r.origin - s.center;
	f32 a = Dot(r.dir, r.dir);
	f32 b = 2 * Dot(r.dir, oc);
	f32 c = Dot(oc, oc) - s.radius * s.radius;
	f32 delta = b * b - 4 * a*c;

	if (delta > 0) {
		f32 sqrDelta = sqrt(delta);

		float t = (-b - sqrDelta)/(2*a);
		if (t > tmin && t < tmax) {
			outHit.pos = r.PointAt(t);
			outHit.normal = (outHit.pos - s.center) / s.radius;
			outHit.t = t;
			return true;
		}

		t = (-b + sqrDelta)/(2*a);
		if (t > tmin && t < tmax) {
			outHit.pos = r.PointAt(t);
			outHit.normal = (outHit.pos - s.center) / s.radius;
			outHit.t = t;
			return true;
		}

	}

	return false;
}

#include <AABB.h>

b32 hitAABB(const Ray& r, const Sphere& s, f32 tmin, f32 tmax, Hit& outHit) {
	AABB aabb(s.center - vec3(s.radius), s.center + vec3(s.radius));

	if (aabb.hit(r, tmin, tmax)) {
		if (HitSphere(r, s, tmin, tmax, outHit)) {
			return true;
		}
	}

	return false;
}