#pragma once
#include <Ray.h>
#include <math.h>

struct Camera {
	explicit Camera(vec3 origin, vec3 lookat, f32 fovyDeg, f32 aspect, f32 aperture, f32 focusDist, vec3 camUp = vec3(0, 1, 0))
	: origin(origin) {
		// lookat
		w = Normalized(origin-lookat);
		u = Normalized(Cross(camUp, w));
		v = Cross(w, u);

		// focus
		lensRadius = aperture/2;

		// film
		f32 theta = fovyDeg * DEG2RAD;
        f32 halfHeight = tan(theta/2);
        f32 halfWidth = aspect * halfHeight;
		lowerLeft = origin -halfWidth*focusDist*u - halfHeight*focusDist*v - focusDist*w;
        horizontal = 2*focusDist*halfWidth*u;
        vertical = 2*focusDist*halfHeight*v;
	}

	Ray GetRay(f32 s, f32 t) {
		vec3 rd = lensRadius * RandomInUnitDisk();
		vec3 offset = u * rd.x + v * rd.y;
		vec3 start = origin+offset;
		return Ray(start, lowerLeft + s * horizontal + t * vertical - start);
	}

	vec3 origin;
	vec3 lowerLeft;
	vec3 horizontal;
	vec3 vertical;
	vec3 w, u, v;
	f32 lensRadius;
};