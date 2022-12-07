#pragma once

class RayTracer
{
private:
	// Multithreading
	bool multithreadingEnabled = MULTITHREADING;
	unsigned int nThreads = processor_count * 2;
	unsigned int threadWidth = SCRWIDTH / nThreads;
	vector<int> threadStartPoints;
	ThreadPool threadPool;
protected:
	Scene* scene;
	float uvX;
	float uvY;
	unsigned int maxBounces = MAX_RECURSIONS;

	// MSAA
	uint AASamples = (clamp(MSAA_SAMPLES, 1, 4) * clamp(MSAA_SAMPLES, 1, 4));

	virtual void Render(Color renderBuffer[SCRWIDTH][SCRHEIGHT], unsigned int yStart, unsigned int yEnd) = 0;
	virtual Color Trace(Ray& ray, unsigned int bounceDepth = 0) = 0;
public:
	float2 uv[SCRWIDTH][SCRHEIGHT];
	Camera* cam;

	RayTracer(Scene* scene, Camera* cam);
	~RayTracer() {};

	virtual void Render(Color renderBuffer[SCRWIDTH][SCRHEIGHT]);
	Ray GetUVRay(const float2& uv) const;
	virtual void OnCameraUpdate() { }
};
