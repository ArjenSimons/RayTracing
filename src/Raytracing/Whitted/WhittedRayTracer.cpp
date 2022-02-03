#include "precomp.h"


void WhittedRayTracer::Render(Color renderBuffer[SCRWIDTH][SCRHEIGHT], unsigned int xStart, unsigned int xEnd)
{
	// Starting at xStart width, until xEnd width, trace vertical segments of the screen.
	for (unsigned int i = xStart; i < xEnd; ++i) for (unsigned int j = 0; j < SCRHEIGHT; ++j)
	{
		Color col;
		
		// For every sample, trace a color and add to the total color.
		for (uint aa_idx = 0; aa_idx < AASamples; aa_idx++) {
			Ray ray = GetUVRay(uv[i][j]);
			col += Trace(ray);
		}

		// Divide the color by the amount of samples.
		if (AASamples > 1) {
			float aa_coeff = 1.f / AASamples;
			col *= aa_coeff;
		}
		renderBuffer[i][j] = col;
	}
}

Color WhittedRayTracer::Trace(Ray& ray, unsigned int bounceDepth)
{
	float3 black(0, 0, 0);
	// TODO: set sky color in camera (or scene).
	float3 sky(.4, .7, .8);

	Intersection intersection = scene->Intersect(ray);

	if (!intersection.intersect)
	{
		return sky;
	}

	if (bounceDepth > MAX_RECURSIONS/* || ray.e <= .1f*/) {
		return black;
	}

	float s = intersection.mat.specularity * ray.e;
	float d = 1 - s;

	Color environment = float3(0, 0, 0);

	if (intersection.sTo == SOLID) {
		// Specular reflection.
		if (s != 0 && ray.e > .1)
		{
			environment.value += s * Trace(Ray(intersection.position, reflect(ray.Dir, intersection.normal), s, ray.substance), bounceDepth + 1).value;
		}

		// Diffuse reflection.
		if (d != 0)
		{
			environment.value += d * DirectIllumination(intersection.position, intersection.normal).value;
		}
	}

	// Every other substance type is for dielectrics.
	if (intersection.sTo != SOLID)
	{
		environment.value += TraceDielectrics(ray, intersection, bounceDepth).value;
	}

	// Get material color, or texture color if applicable.
	Color C = intersection.mat.GetColor(intersection.uv, intersection.position) * environment;
	return C;

	// TODO: Remove this if we don't need it anymore.
	// -----------------------------------------------------------
	//zBuffer
	// -----------------------------------------------------------
	//return float3(1 / intersection.position.z, 1 / intersection.position.z, 1 / intersection.position.z) / 2;
}

Color WhittedRayTracer::DirectIllumination(float3 pos, float3 N)
{
	Color out = float3(0, 0, 0);

	//float cosa = clamp(dot(N, normalize(-float3(.2f, -.8f, .1f))), 0.0, 1.0);

	//return out * cosa;

	for (LightSource* light : scene->GetLights())
	{
		float3 C = light->GetDir(pos);

		float d2 = dot(C, C);
		Ray ray(pos, normalize(C));

		float cosa = clamp(dot(N, ray.Dir), 0.0, 1.0);

		if (cosa == 0 || RayIsBlocked(ray, d2, light)){ continue; } //Go to next light source when ray is blocked
		float3 col = light->color.value;
		col *= 1 / d2;
		col *= cosa;
		col *= light->intensity;

		out.value += col;
	}
	return out;
}

Color WhittedRayTracer::TraceDielectrics(const Ray& ray, const Intersection& i, unsigned int bounceDepth)
{
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

	// Reflection
	if (Fr > 0) {
		//Reflect 
		float3 reflectDir = Reflect(ray.Dir, i.normal);
		Color reflectCol = Fr * Trace(Ray(i.position, reflectDir, terms.energy * Fr, ray.substance), bounceDepth + 1).value;
		color += reflectCol;
	}

	// Transmission (refraction)
	if (Ft > 0) {
		float3 refractDir = Refract(ray.Dir, i.normal, terms);
		Color refractCol = Ft * Trace(Ray(i.position, refractDir, terms.energy * Ft, i.sTo), bounceDepth + 1).value;
		color += refractCol;
	}

	return color;
}

bool WhittedRayTracer::RayIsBlocked(Ray& ray, float d2, LightSource* l) const
{
	bool isDirectional = typeid(*l) == typeid(DirectionalLight);
	bool isSpotLight = typeid(*l) == typeid(SpotLight);

	Intersection i = scene->Intersect(ray);

	if (isDirectional)
	{
		if (i.t > 0.001f && i.intersect)
			return true;
	}
	else if (i.t > 0.001f && i.t * i.t < d2)
	{
		return true;
	}

	// If we're dealing with a spotlight,
	// make sure we are not approaching the light at an angle
	// greater than the angle of the 'hood'
	if (isSpotLight)
	{
		return l->IsBlocked(ray.Dir);
	}
	return false;
}