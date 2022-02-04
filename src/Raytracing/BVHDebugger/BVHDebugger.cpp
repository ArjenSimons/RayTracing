#include "precomp.h"

#define RED_COST_COUNT 10

// Converted from python to c++. Original at https://stackoverflow.com/a/20792531
inline Color rgb(float minimum, float maximum, float value) {
	float ratio = 2 * ((value - minimum) / (maximum - minimum));
	float r = fmax(0, ratio - 1);
	float b = fmax(0, 1 - ratio);
	float g = 1.f - b - r;
	return Color(r, g, b);
}

Color BVHDebugger::Trace(Ray& ray, unsigned int bounceDepth) {
	Color col = float3(0.f,0.f,0.f);
	Intersection i = scene->GetBVH()->Traverse(ray);
	float maxCount = RED_COST_COUNT;
	return rgb(0, maxCount, i.nAABBandTriChecks);
}

void BVHDebugger::Render(Color renderBuffer[SCRWIDTH][SCRHEIGHT], unsigned int xStart, unsigned int xEnd) {
	for (unsigned int i = xStart; i < xEnd; ++i) for (unsigned int j = 0; j < SCRHEIGHT; ++j)
		renderBuffer[i][j] = Trace(GetUVRay(uv[i][j]));
}
