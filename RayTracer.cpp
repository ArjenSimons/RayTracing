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
	renderBuffer = std::vector<std::vector<float3>>(SCRWIDTH, std::vector<float3>(SCRHEIGHT, float3(0, 0, 0)));

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

std::vector<std::vector<float3>> RayTracer::Render()
{
	float3 white(1, 1, 1);
	float3 black(0, 0, 0);
	float3 sky(.2, .2, .2);

	bool printed = false;

	Ray ray;
	for (int i = 0; i < SCRWIDTH; i++) for (int j = 0; j < SCRHEIGHT; j++)
	{
		ray = GetUVRay(uv[i][j]);

		Intersection* closest_intersection = nullptr;

		for (Intersectable* obj : m_scene.GetObjects())
		{
			Intersection* intersection;
			intersection = &obj->Intersect(ray);

			if (intersection->intersect)
			{
				if (!printed) {
					printf("%f | %f | %f \n", intersection->position.x, intersection->position.y, intersection->position.z);
					printed = true;
				}

				closest_intersection = intersection;
			}
		}

		if (closest_intersection == nullptr)
		{
			renderBuffer[i][j] = sky;
		}
		else
		{
			// -----------------------------------------------------------
			//zBuffer
			// -----------------------------------------------------------
			renderBuffer[i][j] = float3(1 / closest_intersection->position.z, 1 / closest_intersection->position.z, 1 / closest_intersection->position.z);

			// -----------------------------------------------------------
			//Checker pattern
			// -----------------------------------------------------------
			//if (((int)(closest_intersection->position.x) + (int)(closest_intersection->position.z)) & 1)
			//{
			//	renderBuffer[i][j] = white;
			//}
			//else
			//{
			//	renderBuffer[i][j] = black;
			//}
		}
	}

	return renderBuffer;
}



Ray RayTracer::GetUVRay(float2 uv)
{
	float3 dir = normalize(p0 + uv.x * (p1 - p0) + uv.y * (p2 - p0));

	return Ray(camPos, dir, 100);
}