#pragma once

class RayTracer
{
private:
	float2 uv[SCRWIDTH][SCRHEIGHT];
	float uvX;
	float uvY;
	float df;

	Color renderBuffer[SCRWIDTH][SCRHEIGHT];
	Color tempBuffer[SCRWIDTH][SCRHEIGHT];

	unsigned int nThreads = processor_count * 4;
	ThreadingStatus threadingStatus;
	ThreadPool threadPool;
	unsigned int threadWidth = SCRWIDTH / nThreads;
	std::vector<int> threadStartPoints;

	MSAA msaaStatus;

	Scene* scene;
	unsigned int maxBounces;
public:
	Camera cam;
	//RayTracer();
	RayTracer(Scene* scene, float distortion, unsigned int maxBounces, ThreadingStatus threadingStatus, MSAA msaaStatus);
	~RayTracer();

	void SetScene(Scene* scene);

	void Render();
	void Render(unsigned int yStart, unsigned int yEnd);
	Color GetBufferValue(int& i, int& j) const { return renderBuffer[i][j]; }
	Color Trace(Ray &ray, unsigned int bounceDepth = 0);

	float2 GetUV(int x, int y) const { return uv[x][y]; }
	Ray GetUVRay(const float2& uv) const;
	Scene* GetScene() { return scene; }
private:
	Intersection GetNearestIntersection(Ray& ray);
	Color DirectIllumination(float3 point, float3 normal);
	Color Refraction(const Ray& ray, const Intersection& intersection, unsigned int bounceDepth);
	float3 Reflect(const float3& dir, const float3& normal) const;
	bool RayIsBlocked(Ray& ray, float d2, LightSource* l) const;
};