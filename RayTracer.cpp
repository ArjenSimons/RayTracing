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

	Ray ray;
	for (int i = 0; i < SCRWIDTH; i++) for (int j = 0; j < SCRHEIGHT; j++)
	{
		ray = GetUVRay(uv[i][j]);

		for (Intersectable* obj : m_scene.GetObjects())
		{
			Intersection* intersection = &obj->Intersect(ray);
			if (intersection->intersect) 
			{
				if (((int)(intersection->position.x) + (int)(intersection->position.z)) & 1)
				{
					renderBuffer[i][j] = white;
				}
				else 
				{
					renderBuffer[i][j] = black;
				}
			}
			else 
			{
				renderBuffer[i][j] = sky;
			}
		}
	}

	return renderBuffer;
}



Ray RayTracer::GetUVRay(float2 uv) 
{
	float3 dir = normalize(p0 + uv.x * (p1 - p0) + uv.y * (p2 - p0));

	return Ray(camPos, dir, 100);
}