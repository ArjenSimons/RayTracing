#include "precomp.h"

void PathTracer::Render(Color renderBuffer[SCRWIDTH][SCRHEIGHT]) {
	// Increment accumulator before allowing parent class to take over again.
	accumulator.incrementAccumulation();
	RayTracer::Render(renderBuffer);
}

void PathTracer::Render(Color renderBuffer[SCRWIDTH][SCRHEIGHT], unsigned int xStart, unsigned int xEnd)
{
	// Starting at xStart width, until xEnd width, trace vertical segments of the screen.
	for (unsigned int i = xStart; i < xEnd; ++i) for (unsigned int j = 0; j < SCRHEIGHT; ++j)
	{
		// For every sample, trace a color and add to the total color.
		Ray ray = GetUVRay(uv[i][j]);
		renderBuffer[i][j] = accumulator.accumulateEnergy(Trace(ray), i, j);;
	}
}

Color PathTracer::Trace(Ray& ray, unsigned int bounceDepth) {
	// Trace ray for a new sample.
	Intersection i = scene->Intersect(ray);
	
	float3 black(0, 0, 0);
	// TODO: set sky color in camera (or scene).
	float3 sky(.4, .7, .8);

	// End recursion if no collision was found.
	if (i.intersect == false)
		return sky;

	// We found a light source, end recursion and return light emission value.
	if (i.sTo == LIGHT)
		return i.mat.GetEmission();

	// Reached max recursion without finding the background or a light source, so we return black.
	if (bounceDepth > MAX_RECURSIONS)
		return float3(0, 0, 0);

	// We hit a specular/diffuse material.
	if (i.sTo == SOLID) {

		// Randomly determine whether we'll choose specular or diffuse reflection based on the material's specular value.
		const float specular = i.mat.specularity;
		float rand_float = RandomFloat();
		bool specular_reflect = rand_float < specular;

		Color albedo = i.mat.GetColor(i.uv, i.position);

		// Specular samples are reflected using the incident direction and the normal.
		if (specular_reflect) {
			Ray specular_ray(i.position, reflect(ray.Dir, i.normal), 1);
			return albedo * Trace(specular_ray, bounceDepth + 1);
		}
		// Diffuse samples are reflected randomly across the hemisphere.
		else {
			float diffuse = 1 - specular;

			float3 diffuse_dir = diffuseReflection(i.normal);
			Color BRDF = (albedo / PI);
			Ray diffuse_ray(i.position, diffuse_dir);

			// Update throughput
			Color Ei = Trace(diffuse_ray, bounceDepth + 1) * dot(i.normal, diffuse_dir);

			return BRDF * Ei * (PI * 2.0f);
		}
	}

	// We hit a dielectric material.
	if (i.sTo != SOLID && i.sTo != LIGHT) {

		// Precalculate variables for refraction and fresnel formulas.
		DielectricTerms terms = DielectricTerms(ray, i);

		// Reflection and transmission coeficcients.
		float Fr, Ft;

		// TIR: Total internal reflection. No need to use fresnel for Fr and Ft.
		if (terms.k < 0) {
			Fr = 1.f;
			Ft = 0.f;
		}
		// No TIR, use fresnel to determine Fr and Ft.
		else {
			Fresnel(terms, Fr, Ft);
		}

		Color color = Color();

		float rand_float = RandomFloat();
		bool specularReflect = rand_float < Fr;

		// Reflection
		if (specularReflect) {
			//Reflect 
			float3 reflectDir = Reflect(ray.Dir, i.normal);
			Color reflectCol = Trace(Ray(i.position, reflectDir, terms.energy, ray.substance), bounceDepth + 1).value;
			return reflectCol;
		}
		else {
			float3 refractDir = Refract(ray.Dir, i.normal, terms);
			Color refractCol = Trace(Ray(i.position, refractDir, terms.energy, i.sTo), bounceDepth + 1).value;
			return refractCol;
		}
	}

	return float3(0, 0, 0);
};
