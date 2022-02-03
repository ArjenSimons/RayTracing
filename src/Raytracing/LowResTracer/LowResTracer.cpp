#include "precomp.h"

void LowResTracer::Render(Color renderBuffer[SCRWIDTH][SCRHEIGHT], unsigned int xStart, unsigned int xEnd)
{
	// Starting at xStart width, until xEnd width, trace vertical segments of the screen.
	for (unsigned int i = xStart; i < xEnd; i += pixel_size) for (unsigned int j = 0; j < SCRHEIGHT; j += pixel_size)
	{
		// For every pixel_size sized group of pixels, trace a color and add to the total color.
		Ray ray = GetUVRay(uv[i][j]);
		Color col = Trace(ray);
		for (uint px_i = i; px_i < i + pixel_size; px_i++) for (uint px_j = j; px_j < j + pixel_size; px_j++) {
			renderBuffer[px_i][px_j] = col;
		}
	}
}

Color LowResTracer::Trace(Ray& ray, uint bounceCount) {
	Intersection intersection = scene->Intersect(ray);

	float3 black(0, 0, 0);
	// TODO: set sky color in camera (or scene).
	float3 sky(.4, .7, .8);

	if (!intersection.intersect) {
		return sky;
	}

	if (intersection.sTo == SOLID) {

		float illumination = clamp(dot(intersection.normal, normalize(-float3(.2f, -.8f, .1f))), 0.0, 1.0);
		return intersection.mat.GetColor(intersection.uv, intersection.position) * illumination;
	}

	return black;
}