#pragma once
#include "Scene.h"

class RayTracer
{
private:
	//Cam
	float3 camPos = float3(0, 0, 0);
	float3 viewDir = float3(0, 0, 1);
	float d = 1;
	float3 C = camPos + d * viewDir;
	float3 p0 = C + float3(-1, -1, 0);
	float3 p1 = C + float3( 1, -1, 0);
	float3 p2 = C + float3(-1,  1, 0);

	float2 uv[SCRWIDTH][SCRHEIGHT];
	std::vector<std::vector<float3>> renderBuffer;

	Scene m_scene;
public:
	//RayTracer();
	RayTracer(Scene scene);
	~RayTracer();

	void SetScene(Scene scene);

	std::vector<std::vector<float3>> Render();

	Ray GetUVRay(float2 uv);
};


