#include "precomp.h"
#include "RayTracer.h"

//RayTracer::RayTracer()
//{
//	renderBuffer = std::vector<std::vector<float3>>(SCRWIDTH, std::vector<float3>(SCRHEIGHT, float3(0, 0, 0)));
//
//	//for (int i = 0; i < SCRWIDTH; i++) for (int j = 0; j < SCRHEIGHT; j++)
//	//{
//	//	uv[i][j] = float2(static_cast<float>(i) / static_cast<float>(SCRWIDTH), static_cast<float>(j) / static_cast<float>(SCRHEIGHT));
//	//}
//}

RayTracer::RayTracer(Scene scene, unsigned int maxBounces, ThreadingStatus threadingStatus)
	: scene(scene), maxBounces(maxBounces), threadingStatus(threadingStatus), threadPool(processor_count)
{
	//renderBuffer = std::vector<std::vector<float3>>(SCRWIDTH, std::vector<float3>(SCRHEIGHT, float3(0, 0, 0)));
	cam = Camera();

	for (int i = 0; i < SCRWIDTH; i++) for (int j = 0; j < SCRHEIGHT; j++)
	{
		uv[i][j] = float2(static_cast<float>(i) / static_cast<float>(SCRWIDTH), static_cast<float>(j) / static_cast<float>(SCRHEIGHT));
	}

	for (unsigned int i = 0; i <= nThreads; i++)
	{
		threadStartPoints.emplace_back(threadWidth * i);
	}
}

RayTracer::~RayTracer()
{
}

void RayTracer::SetScene(Scene scene)
{
	scene = scene;
}

void RayTracer::Render()
{
	if (threadingStatus == THREADING_ENABLED)
	{
		std::vector<std::future<void>> results;

		for (unsigned int i = 0; i < nThreads; ++i)
		{
			results.emplace_back(
				threadPool.enqueue([this, i] { Render(threadStartPoints[i], threadStartPoints[i + 1]); })
			);
		}

		//Wait untill all threads are finished
		threadPool.wait_until_empty();
		threadPool.wait_until_nothing_in_flight();
	}
	else 
	{
		for (int i = 0; i < SCRWIDTH; ++i) for (int j = 0; j < SCRHEIGHT; ++j)
		{
			Ray ray = GetUVRay(uv[i][j]);
			renderBuffer[i][j] = Trace(ray).GetRGBValue();
		}
	}
}

void RayTracer::Render(unsigned int xStart, unsigned int xEnd)
{
	for (unsigned int i = xStart; i < xEnd; ++i) for (unsigned int j = 0; j < SCRHEIGHT; ++j)
	{
		Ray ray = GetUVRay(uv[i][j]);
		renderBuffer[i][j] = Trace(ray).GetRGBValue();
	}
}

Color RayTracer::Trace(Ray& ray, unsigned int bounceDepth)
{
	float3 black(0, 0, 0);
	float3 sky(.4, .7, .8);

	Intersection intersection = GetNearestIntersection(ray);

	if (!intersection.intersect)
	{
		return sky;
	}
	else
	{
		float s = 0;
		if (bounceDepth <= maxBounces && ray.e > .1f)
		{
			s = intersection.mat.specularity * ray.e;
		}
		else 
		{
			return black;
		}

		float d = 1 - s;

		Color environment = float3(0, 0, 0);

		if (intersection.sTo != SOLID)
		{
			environment.value += Refraction(ray, intersection, bounceDepth).value;
		}
		else 
		{
			if (s != 0 && ray.e > .1)
			{
				environment.value += s * Trace(Ray(intersection.position, reflect(ray.Dir, intersection.normal), s, ray.substance), bounceDepth + 1).value;
			}
			if (d != 0)
			{
				environment.value += d * DirectIllumination(intersection.position, intersection.normal).value;
			}
		}

		return intersection.mat.GetColor(intersection.uv, intersection.position).value * environment.value;

		// -----------------------------------------------------------
		//zBuffer
		// -----------------------------------------------------------
		//return float3(1 / intersection.position.z, 1 / intersection.position.z, 1 / intersection.position.z) / 2;
	}
}

Intersection RayTracer::GetNearestIntersection(Ray& ray)
{
	Intersection closest_intersection;

	for (Intersectable* obj : scene.GetObjects())
	{
		Intersection* intersection;
		intersection = &obj->Intersect(ray);

		if (intersection->intersect && intersection->t != 0 && intersection->t < closest_intersection.t)
		{
			closest_intersection = *intersection;
		}
	}
	return closest_intersection;
}

Color RayTracer::DirectIllumination(float3 pos, float3 N)
{
	Color out = float3(0, 0, 0);

	for (LightSource* light : scene.GetLights())
	{
		float3 C = light->position - pos;

		float d2 = dot(C, C);
		Ray ray(pos, normalize(C));

		if (RayIsBlocked(ray, d2)){ continue; } //Go to next light source when ray is blocked

		float3 col = light->color.value;
		col *= 1 / d2;
		col *= clamp(dot(N, ray.Dir), 0.0, 1.0);
		col *= light->intensity;

		out.value += col;
	}
	return out;
}

Color RayTracer::Refraction(const Ray& ray, const Intersection& i, unsigned int bounceDepth)
{
	float n1 = RefractionIndex(ray.substance);
	float n2 = RefractionIndex(i.sTo);

	float indexRatio = n1/ n2;
	float3 D = ray.Dir * -1;
	float cosi = dot(i.normal, D);

	float k = 1 - ((indexRatio * indexRatio) * (1 - cosi * cosi));

	float energy = ray.e * exp(-Absorption(ray.substance) * i.t);

	if (k < 0) //TIR
	{
		return Trace(Ray(i.position, Reflect(ray.Dir, i.normal), energy, ray.substance), bounceDepth + 1);
	}

	//Fresnel
	float sint = indexRatio * sqrtf(max(0.f, 1 - cosi * cosi));
	float cost = sqrt(max(0.0f, 1 - sint * sint));
	cosi = fabsf(cosi);

	float n1TimesAngle1 = n1 * cosi;
	float n1TimesAngle2 = n1 * cost;
	float n2TimesAngle1 = n2 * cosi;
	float n2TimesAngle2 = n2 * cost;

	float sPolarizedSqrd = (n1TimesAngle1 - n2TimesAngle2) / (n1TimesAngle1 + n2TimesAngle2);
	float pPolarizedSqrd = (n1TimesAngle2 - n2TimesAngle1) / (n1TimesAngle2 + n2TimesAngle1);

	float Fr = energy * .5f * (sPolarizedSqrd * sPolarizedSqrd + pPolarizedSqrd * pPolarizedSqrd);
	float Ft = energy - Fr;


	//Reflect 
	Color reflect = Fr * Trace(Ray(i.position, Reflect(ray.Dir, i.normal), energy * Fr, ray.substance), bounceDepth + 1).value;

	//Refract
	float3 dir = indexRatio * ray.Dir + i.normal * (indexRatio * cosi - sqrt(k));
	Color refract = Ft * Trace(Ray(i.position, dir, energy, i.sTo), bounceDepth).value;

	return refract + reflect;
}

float3 RayTracer::Reflect(const float3& dir, const float3& N) const
{
	return dir - 2 * dot(dir, N) * N;
}

bool RayTracer::RayIsBlocked(Ray& ray, float d2) const
{
	for (Intersectable* object : scene.GetObjects())
	{
		Intersection i = object->Intersect(ray);

		if (i.t > 0 && i.t * i.t < d2)
		{
			return true;
		}
	}
	return false;
}

Ray RayTracer::GetUVRay(const float2& uv) const
{
	return Ray(cam.pos, normalize((cam.p0 + uv.x * (cam.p1 - cam.p0) + uv.y * (cam.p2 - cam.p0)) - cam.pos), 1, AIR, 0, 100);
}