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
	: m_scene(scene)
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
	m_scene = scene;
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

Color RayTracer::Trace(Ray &ray) 
{
	float3 white(1, 1, 1);
	float3 black(0, 0, 0);
	float3 sky(.2, .2, .2);

	Intersection intersection = GetNearestIntersection(ray);

	if (!intersection.intersect)
	{
		return sky;
	}
	else
	{
		//Color out = intersection.mat.GetColor(intersection.position).value * DirectIllumination(intersection.position, intersection.normal).value;
		
		return intersection.mat.GetColor(intersection.position).value * DirectIllumination(intersection.position, intersection.normal).value;

		// -----------------------------------------------------------
		//zBuffer
		// -----------------------------------------------------------
		//return float3(1 / intersection.position.z, 1 / intersection.position.z, 1 / intersection.position.z) / 2;
	}
}

Intersection RayTracer::GetNearestIntersection(Ray& ray) 
{
	Intersection closest_intersection;

	for (Intersectable* obj : m_scene.GetObjects())
	{
		Intersection* intersection;
		intersection = &obj->Intersect(ray);

		if (intersection->intersect && intersection->t < closest_intersection.t)
		{
			closest_intersection = *intersection;
		}
	}

	return closest_intersection;
}

Color RayTracer::DirectIllumination(float3 pos, float3 N)
{
	Color out = float3(0, 0, 0);

	for (LightSource* light : m_scene.GetLights())
	{
		float3 C = light->position - pos;
		float d2 = dot(C, C);
		Ray ray(pos, normalize(C));

		for (Intersectable* object : m_scene.GetObjects())
		{
			Intersection i = object->Intersect(ray);
			if (i.t > 0 && i.t * i.t > d2)
			{
				float3 col = light->color.value;
				col *= 1 / d2;
				col *= clamp(dot(N, ray.Dir), 0.0, 1.0);

				out.value += col;
				break;
			}
		}
	}
	return out;
}

Ray RayTracer::GetUVRay(float2 uv)
{
	float3 dir = normalize(p0 + uv.x * (p1 - p0) + uv.y * (p2 - p0));

	return Ray(camPos, dir, 100);
}