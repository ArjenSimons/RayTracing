#pragma once
#include "Scene.h"
#include "Camera.h"

class RayTracer
{
private:
	//Cam
	Camera cam;

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
	Intersection GetNearestIntersection(Ray &ray);
	Color DirectIllumination(float3 point, float3 normal);

	float2 GetUV(int x, int y) { return uv[x][y]; }
	Ray GetUVRay(float2 uv);
};
