#pragma once
#include "Scene.h"
#include "Camera.h"
#include "ThreadPool.h";

class RayTracer
{
private:
	//Cam
	

	float2 uv[SCRWIDTH][SCRHEIGHT];
	float uvX;
	float uvY;
	unsigned int renderBuffer[SCRWIDTH][SCRHEIGHT];

	unsigned int nThreads = 16;
	ThreadingStatus threadingStatus;
	ThreadPool threadPool;
	unsigned int threadWidth = SCRWIDTH / nThreads;
	std::vector<int> threadStartPoints;

	Scene scene;
	unsigned int maxBounces;
public:
	Camera cam;
	//RayTracer();
	RayTracer(Scene scene, unsigned int maxBounces, ThreadingStatus threadingStatus);
	~RayTracer();

	void SetScene(Scene scene);

	void Render();
	void Render(unsigned int yStart, unsigned int yEnd);
	unsigned int GetBufferValue(int& i, int& j) const { return renderBuffer[i][j]; }
	Color Trace(Ray &ray, unsigned int bounceDepth = 0);

	float2 GetUV(int x, int y) const { return uv[x][y]; }
	Ray GetUVRay(const float2& uv) const;
private:
	Intersection GetNearestIntersection(Ray& ray);
	Color DirectIllumination(float3 point, float3 normal);
	Color Refraction(const Ray& ray, const Intersection& intersection, unsigned int bounceDepth);
	float3 Reflect(const float3& dir, const float3& normal) const;
	bool RayIsBlocked(Ray& ray, float d2) const;
};
