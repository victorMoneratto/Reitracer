#include <iostream>
#include <iomanip>
#include <cfloat>
#include <cstdlib>
#include <ctime>

#include <Hit.h>
#include <Camera.h>

#include <windows.h>
#include <omp.h>

const f32 R = sqrtf(10)/2;
Sphere s_defaultSpheres[] = {
	{vec3(0,-100.5,-1), 100},
    {vec3(2,0,-1), 0.5f},
    {vec3(0,0,-1), 0.5f},
    {vec3(-2,0,-1), 0.5f},
    {vec3(2,0,1), 0.5f},
    {vec3(0,0,1), 0.5f},
    {vec3(-2,0,1), 0.5f},
    {vec3(0.5f,1,0.5f), 0.5f},
    {vec3(-1.5f,1.5f,0.f), 0.3f},
};
const i32 kNumDefaultSpheres = sizeof(s_defaultSpheres) / sizeof(*s_defaultSpheres);

Material s_defaultMats[kNumDefaultSpheres] = {
	{ Material::Lambert, vec3(0.8f, 0.8f, 0.8f), 0, 0, },
    { Material::Lambert, vec3(0.8f, 0.4f, 0.4f), 0, 0, },
    { Material::Lambert, vec3(0.4f, 0.8f, 0.4f), 0, 0, },
    { Material::Metal, vec3(0.4f, 0.4f, 0.8f), 0, 0 },
    { Material::Metal, vec3(0.4f, 0.8f, 0.4f), 0, 0 },
    { Material::Metal, vec3(0.4f, 0.8f, 0.4f), 0.2f, 0 },
    { Material::Metal, vec3(0.4f, 0.8f, 0.4f), 0.6f, 0 },
    { Material::Dielectric, vec3(0.4f, 0.4f, 0.4f), 0, 1.5f },
    { Material::Lambert, vec3(0.8f, 0.6f, 0.2f), 0, 0 },
};

const i32 kNumCoverSpheres = 500;
Sphere s_coverSpheres[kNumCoverSpheres];
Material s_coverMats[kNumCoverSpheres];
#define DO_COVER false

struct Scene {
	Sphere* spheres;
	Material* sphereMats;
	i32 numSpheres;
};

Scene defaultScene = {s_defaultSpheres, s_defaultMats, kNumDefaultSpheres};
Scene coverScene = {s_coverSpheres, s_coverMats, kNumCoverSpheres};

b32 HitWorld(const Ray& r, const Scene& scene, float tmin, float tmax, Hit& outHit, i32& outID) {
	b32 hitAnything = false;
	f32 closest = tmax;
	for (i32 i = 0; i < scene.numSpheres; i++) {
		Hit hitResult;
		if (hitAABB(r, scene.spheres[i], tmin, closest, hitResult)) {
			hitAnything = true;
			closest = hitResult.t;
			outHit = hitResult;
			outID = i;
		}
	}

	return hitAnything;
}

b32 Scatter(const Material& mat, const Ray& r, const Hit& hit, vec3& outAttenuation, Ray& outScattered) {
	if (mat.type == Material::Lambert) {
		vec3 target = hit.pos + hit.normal + RandomInUnitSphere();
		outScattered = Ray(hit.pos, target - hit.pos);
		outAttenuation = mat.albedo;
		return true;
	} else if (mat.type == Material::Metal) {
		vec3 reflected = Reflect(Normalized(r.dir), hit.normal + mat.roughness * RandomInUnitSphere());
		outScattered = Ray(hit.pos, reflected);
		outAttenuation = mat.albedo;
		return Dot(reflected, hit.normal) > 0;
	} else if (mat.type == Material::Dielectric) {
		vec3 outwardNormal;
		f32 nint;
		f32 cos;
		if (Dot(r.dir, hit.normal) > 0) {
			outwardNormal = -hit.normal;
			nint = mat.ri;
			cos = mat.ri * Dot(r.dir, hit.normal) / r.dir.Length();
		} else {
			outwardNormal = hit.normal;
			nint = 1.f / mat.ri;
			cos = -Dot(r.dir, hit.normal) / r.dir.Length();
		}

		outAttenuation = vec3(1.f);
		vec3 refracted;
		f32 reflectProbability;
		if (Refract(r.dir, outwardNormal, nint, refracted)) {
			reflectProbability = Schlick(cos, mat.ri);
		} else {
			reflectProbability = 1;
		}

		if (RandFloat01() < reflectProbability) {
			vec3 reflected = Reflect(r.dir, hit.normal);
			outScattered = Ray(hit.pos, reflected);
		} else {
			outScattered = Ray(hit.pos, refracted);
		}

		return true;
	}

	return false;
}

#define MAX_BOUNCES 20

vec3 Color(const Ray& r, const Scene& scene, i32& depth) {
	Hit hit;
	i32 id;
	depth += 1;
	if (HitWorld(r, scene, 1e-3f, FLT_MAX, hit, id)) {
		Ray scattered;
		vec3 attenuation;
		const Material& mat = scene.sphereMats[id];
		if (depth < MAX_BOUNCES && Scatter(mat, r, hit, attenuation, scattered)) {
			return attenuation * Color(scattered, scene, depth);
		} else {
			// no more bounces
			return vec3(0, 0, 0);
		}
		
	}

	vec3 dir = Normalized(r.dir);
	f32 alpha = .5f * (dir.y + 1);
	return Lerp(vec3(1, 1, 1), vec3(0.5f, 0.7f, 1.0f), alpha);
}


int main() {
	const i32 nx = 1280;
	const i32 ny = 720;
	i32 ns = 64;

	union backbuffer_t {
		struct {
			u8 r, g, b;
		};
		u8 v[3];
	};

	backbuffer_t* backbuffer = new backbuffer_t[nx * ny];

	srand((u32)time(nullptr)); // REMOVE THIS

	vec3 origin = vec3(0, 2, 3);
	vec3 lookat = vec3(0, 0, 0);
	Camera cam(origin, lookat, 60.f, f32(nx)/f32(ny), 0.1f, 3);

#if false && DO_COVER
	{
		s_coverSpheres[0] = { vec3(0, -1000, 0), 1000 };
		s_coverMats[0] = {/*type=*/ Material::Lambert, /*albedo=*/ vec3(.5), /*roughness=*/ 0.f, /*ri=*/ 0.f };

		i32 i = 1;
		for (i32 a = -11; a < 11; a++) {
			for (i32 b = -11; b < 11; b++) {
				f32 chooseMat = RandFloat01();
				vec3 center(a + .9f*RandFloat01(), .2f, b + .9f*RandFloat01());
				if ((center - vec3(4, 0.2f, 0)).Length() > .9f) {
					if (chooseMat < .8f) {
						s_coverSpheres[i++] = Sphere{ center, .2f };
						s_coverMats[i] = {/*type=*/ Material::Lambert,
							/*albedo=*/ vec3(RandFloat01()*RandFloat01(), RandFloat01()*RandFloat01(), RandFloat01()*RandFloat01()),
							/*roughness=*/ 0.f, /*ri=*/ 0.f };
					} else if (chooseMat < .95f) {
						s_coverSpheres[i++] = Sphere{ center, .2f };
						s_coverMats[i] = {/*type=*/ Material::Metal,
							/*albedo=*/ vec3(.5f + .5f*RandFloat01(), .5f + .5f*RandFloat01() , .5f + .5f*RandFloat01()),
							/*roughness=*/ 0.5f * RandFloat01(), /*ri=*/ 0.f };
					} else {
						s_coverSpheres[i++] = Sphere{ center, .2f };
						s_coverMats[i] = {/*type=*/ Material::Dieletric, /*albedo=*/ vec3(1.f), /*roughness=*/ 0.f, /*ri=*/ 1.5f };
					}
				}
			}
		}

		s_coverSpheres[i++] = Sphere{ vec3(0, 1, 0), 1.f };
		s_coverMats[i] = {/*type=*/ Material::Dieletric, /*albedo=*/ vec3(1.f), /*roughness=*/ 0.f, /*ri=*/ 1.5f };

		s_coverSpheres[i++] = Sphere{ vec3(-4, 1, 0), 1.f };
		s_coverMats[i] = {/*type=*/ Material::Lambert, /*albedo=*/ vec3(.4f, .2f, .1f), /*roughness=*/ 0.f, /*ri=*/ 0.f };

		s_coverSpheres[i++] = Sphere{ vec3(4, 1, 0), 1.f };
		s_coverMats[i] = {/*type=*/ Material::Metal, /*albedo=*/ vec3(.7f, .6f, .5f), /*roughness=*/ 0.f, /*ri=*/ 0.f };
	}
#endif

	i64 startTime;
	{
		LARGE_INTEGER startTimeLI;
		QueryPerformanceCounter(&startTimeLI);
		startTime = startTimeLI.QuadPart;
	}

	omp_set_dynamic(0);
	omp_set_num_threads(8);

	i64 numRays = 0;
	for (i32 y = ny - 1; y >= 0; y--) {
// 		if (y % 4 == 0) {
// 				std::cerr << "\r" << std::fixed <<
// 					std::setw(3) << std::setprecision(2) <<
// 					(1 -(y/float(ny))) * 100.f;
// 		}
		#pragma omp parallel for reduction(+:numRays)
		for (i32 x = 0; x < nx; x++) {
			vec3 col = vec3(0);
			for (i32 s = 0; s < ns; s++) {
				f32 u = f32(x + RandFloat01()) / (f32)nx;
				f32 v = f32(y + RandFloat01()) / (f32)ny;

				Ray r = cam.GetRay(u, v);

				i32 depth = 0;
				col += Color(r, defaultScene, depth);
				numRays += depth;
			}
			col /= f32(ns);
			col = LinearToSRGB(col);
			
			i32 bbi = x + ((ny - 1 - y)* nx);
			backbuffer[bbi].r = u8(255.99f * col.r);
			backbuffer[bbi].g = u8(255.99f * col.g);
			backbuffer[bbi].b = u8(255.99f * col.b);
		}
	}

	{
		LARGE_INTEGER rayTimeLI;
		QueryPerformanceCounter(&rayTimeLI);
		i64 rayTime = rayTimeLI.QuadPart;

		LARGE_INTEGER freqLI;
		QueryPerformanceFrequency(&freqLI);
		double frequency = double(freqLI.QuadPart);
		double seconds = (rayTime - startTime)/frequency;
		
		std::flush(std::cerr);
		std::cerr << "\nRay time " << std::fixed << std::setprecision(2) << seconds << "s\n";
		std::cerr << std::fixed << std::setprecision(2) << (nx * ny * ns * 1e-6f)/seconds << "M rays/s\n";
	}

	std::cout << "P3\n" << nx << " " << ny << "\n255\n";
	for (i32 i = 0; i < nx * ny; i++) {
		std::cout << i32(backbuffer[i].r) << " " << i32(backbuffer[i].g) << " " << i32(backbuffer[i].b) << "\n";
	}

	delete[] backbuffer;

	return 0;
}