#include "precomp.h"
#include "RayTracer.h"

RayTracer::RayTracer()
{

}

RayTracer::RayTracer(Scene scene)
	: m_scene(scene)
{
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
	float col = 1;
	float back_col = 0;

	std::vector<std::vector<float3>> out(SCRWIDTH, std::vector<float3>(SCRHEIGHT, float3(0, 0, 0)));

	for (int i = 0; i < SCRWIDTH; i++) for (int j = 0; j < SCRHEIGHT; j++)
	{
		Ray ray = GetUVRay(static_cast<float>(i) / static_cast<float>(SCRWIDTH), static_cast<float>(j) / static_cast<float>(SCRHEIGHT));

		for (Intersectable* obj : m_scene.GetObjects())
		{
			if (obj->Intersect(ray)) 
			{
				out[i][j] = float3(col, col, col);
			}
			else 
			{
				out[i][j] = float3(back_col, back_col, back_col);
			}
		}
	}

	return out;
}



Ray RayTracer::GetUVRay(float u, float v) 
{
	float3 dir = normalize(p0 + u * (p1 - p0) + v * (p2 - p0));

	return Ray(camPos, dir, 100);
}