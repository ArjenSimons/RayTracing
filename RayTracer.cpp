#include "precomp.h"
#include "RayTracer.h"

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
	std::vector<std::vector<float3>> out(SCRWIDTH, std::vector<float3>(SCRHEIGHT, float3(0, 0, 0)));

	return out;
}

Ray RayTracer::GetUVRay(float u, float v) 
{
	float3 dir = normalize(p0 + u * (p1 - p0) + v * (p2 - p0));

	return Ray(camPos, dir, 100);
}