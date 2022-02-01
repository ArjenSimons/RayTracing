#include "precomp.h"

RayTracer::RayTracer(Scene* scene, Camera* camera) : scene(scene), cam(camera)
{
	for (int i = 0; i < SCRWIDTH; i++) for (int j = 0; j < SCRHEIGHT; j++)
	{
		uv[i][j] = float2(static_cast<float>(i) / static_cast<float>(SCRWIDTH), static_cast<float>(j) / static_cast<float>(SCRHEIGHT));
	}
	uvX = 1 / static_cast<float>(SCRWIDTH);
	uvY = 1 / static_cast<float>(SCRHEIGHT);

	for (unsigned int i = 0; i <= nThreads; i++)
	{
		threadStartPoints.emplace_back(threadWidth * i);
	}
}

void RayTracer::Render(Color renderBuffer[SCRWIDTH][SCRHEIGHT])
{
	if (MULTITHREADING)
	{
		vector<future<void>> results;

		for (unsigned int i = 0; i < nThreads; ++i)
		{
			results.emplace_back(
				threadPool.enqueue([this, i, renderBuffer] { Render(renderBuffer, threadStartPoints[i], threadStartPoints[i + 1]); })
			);
		}

		//Wait untill all threads are finished
		threadPool.wait_until_empty();
		threadPool.wait_until_nothing_in_flight();
	}
	else
	{
		Render(renderBuffer, 0, SCRWIDTH);
	}
}

Ray RayTracer::GetUVRay(const float2& uv) const
{
	float xOffset = 0;
	float yOffset = 0;

	if (AASamples > 1)
	{
		xOffset = RandomFloat() * uvX;
		yOffset = RandomFloat() * uvY;
	}

	float3 rayDir = normalize((cam->p0 + (uv.x + xOffset) * (cam->p1 - cam->p0) + (uv.y + yOffset) * (cam->p2 - cam->p0)) - cam->pos);
	Ray ray = Ray(cam->pos, rayDir, 1, AIR, 0, 100);

	float df = clamp(LENS_DISTORTION, 0.0f, 1.0f);
	if (df == 0) {
		return ray;
	}

	// Calculate lens distortion
	// indexRatio determines the degree of distortion
	// Because df is clamped on [0..1], 0.5 <= ratio <= 1 will always hold true
	// The same code can in theory be used to implement a pincushion distortion, if a ratio > 1 is used
	// however, our code currently does not support this.
	float indexRatio = 1 / (1 + df);
	float3 D = rayDir * -1;
	float cosi = dot(cam->viewDir, D);
	float k = 1 - ((indexRatio * indexRatio) * (1 - cosi * cosi));
	cosi = fabsf(cosi);

	float3 dir = indexRatio * rayDir + cam->viewDir * (indexRatio * cosi - sqrt(k));
	ray.Dir = normalize(dir);

	return ray;
}