#include "precomp.h"
#include "RayTracer.h"

//RayTracer::RayTracer()
//{
//	renderBuffer = std::vector<std::vector<float3>>(SCRWIDTH, std::vector<float3>(SCRHEIGHT, float3(0, 0, 0)));
//
//	//for (int i = 0; i < SCRWIDTH; i++) for (int j = 0; j < SCRHEIGHT; j++)
//	//{
//	//	uv[i][j] = float2(static_cast<float>(i) / static_cast<float>(SCRWIDTH), static_cast<float>(j) / static_cast<float>(SCRHEIGHT));
//	//}
//}

RayTracer::RayTracer(Scene scene)
	: scene(scene)
{
	//renderBuffer = std::vector<std::vector<float3>>(SCRWIDTH, std::vector<float3>(SCRHEIGHT, float3(0, 0, 0)));

	for (int i = 0; i < SCRWIDTH; i++) for (int j = 0; j < SCRHEIGHT; j++)
	{
		uv[i][j] = float2(static_cast<float>(i) / static_cast<float>(SCRWIDTH), static_cast<float>(j) / static_cast<float>(SCRHEIGHT));
	}
}

RayTracer::~RayTracer()
{
}

void RayTracer::SetScene(Scene scene)
{
	scene = scene;
}

//std::vector<std::vector<float3>> RayTracer::Render()
//{
//	bool printed = false;
//
//	for (int i = 0; i < SCRWIDTH; i++) for (int j = 0; j < SCRHEIGHT; j++)
//	{
//		Ray ray = GetUVRay(uv[i][j]);
//
//		//renderBuffer[i][j] = Trace(ray);
//	}
//
//	return renderBuffer;
//}

Color RayTracer::Trace(Ray& ray)
{
	float3 white(1, 1, 1);
	float3 black(0, 0, 0);
	float3 sky(.4, .7, .8);

	Intersection intersection = GetNearestIntersection(ray);

	if (!intersection.intersect)
	{
		return sky;
	}
	else
	{
		float s = intersection.mat.specularity;
		float d = 1 - s;

		Color environment = float3(0, 0, 0);

		if (s != 0) 
		{
			environment.value += s * Trace(Ray(intersection.position, reflect(ray.Dir, intersection.normal))).value;
		}
		if (d != 0) 
		{
			environment.value += d * DirectIllumination(intersection.position, intersection.normal).value;
		}

		return intersection.mat.GetColor(intersection.position).value * environment.value;

		// -----------------------------------------------------------
		//zBuffer
		// -----------------------------------------------------------
		//return float3(1 / intersection.position.z, 1 / intersection.position.z, 1 / intersection.position.z) / 2;
	}
}

Intersection RayTracer::GetNearestIntersection(Ray& ray)
{
	Intersection closest_intersection;

	for (Intersectable* obj : scene.GetObjects())
	{
		Intersection* intersection;
		intersection = &obj->Intersect(ray);

		if (intersection->intersect && intersection->t != 0 && intersection->t < closest_intersection.t)
		{
			closest_intersection = *intersection;
		}
	}
	return closest_intersection;
}

Color RayTracer::DirectIllumination(float3 pos, float3 N)
{
	Color out = float3(0, 0, 0);

	for (LightSource* light : scene.GetLights())
	{
		float3 C = light->position - pos;

		float d2 = dot(C, C);
		Ray ray(pos, normalize(C));

		if (RayIsBlocked(ray, d2)){ continue; } //Go to next light source when ray is blocked

		float3 col = light->color.value;
		col *= 1 / d2;
		col *= clamp(dot(N, ray.Dir), 0.0, 1.0);
		col *= light->intensity;

		out.value += col;
	}
	return out;
}

float3 RayTracer::Reflect(float3 dir, float3 N) const
{
	return dir - 2 * dot(dir, N) * N;
}

bool RayTracer::RayIsBlocked(Ray& ray, float d2) const
{
	for (Intersectable* object : scene.GetObjects())
	{
		Intersection i = object->Intersect(ray);

		if (i.t > 0 && i.t * i.t < d2)
		{
			return true;
		}
	}
	return false;
}

Ray RayTracer::GetUVRay(float2 uv) const
{
	return Ray(camPos, normalize((p0 + uv.x * (p1 - p0) + uv.y * (p2 - p0)) - camPos), 100);
}