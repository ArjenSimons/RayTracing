#pragma once
#include "Scene.h"

class RayTracer
{
private:
	//Cam
	float3 camPos = float3(0, 0, 0);
	float3 viewDir = float3(0, 0, 1);
	float aspectRatio = (float)SCRWIDTH / (float)SCRHEIGHT;
	float d = 1;
	float3 C = camPos + d * viewDir;
	float3 p0 = C + float3(-1 * aspectRatio,  1, 0);
	float3 p1 = C + float3( 1 * aspectRatio,  1, 0);
	float3 p2 = C + float3(-1 * aspectRatio, -1, 0);

	float2 uv[SCRWIDTH][SCRHEIGHT];
	//std::vector<std::vector<float3>> renderBuffer;

	Scene scene;
public:
	//RayTracer();
	RayTracer(Scene scene);
	~RayTracer();

	void SetScene(Scene scene);

	//std::vector<std::vector<float3>> Render();
	Color Trace(Ray &ray);

	float2 GetUV(int x, int y) const { return uv[x][y]; }
	Ray GetUVRay(float2 uv) const;
private:
	Intersection GetNearestIntersection(Ray& ray);
	Color DirectIllumination(float3 point, float3 normal);
	float3 Reflect(float3 dir, float3 normal) const;
	bool RayIsBlocked(Ray& ray, float d2) const;
};
