#include "precomp.h"
#include "RayTracer.h"
#include <typeinfo>

//RayTracer::RayTracer()
//{
//	renderBuffer = std::vector<std::vector<float3>>(SCRWIDTH, std::vector<float3>(SCRHEIGHT, float3(0, 0, 0)));
//
//	//for (int i = 0; i < SCRWIDTH; i++) for (int j = 0; j < SCRHEIGHT; j++)
//	//{
//	//	uv[i][j] = float2(static_cast<float>(i) / static_cast<float>(SCRWIDTH), static_cast<float>(j) / static_cast<float>(SCRHEIGHT));
//	//}
//}

RayTracer::RayTracer(Scene scene, unsigned int maxBounces, ThreadingStatus threadingStatus, MSAA msaaStatus)
	: scene(scene), maxBounces(maxBounces), threadingStatus(threadingStatus), threadPool(processor_count), msaaStatus(msaaStatus)
{
	//renderBuffer = std::vector<std::vector<float3>>(SCRWIDTH, std::vector<float3>(SCRHEIGHT, float3(0, 0, 0)));
	cam = Camera(float3(0, 0, 0), float3(0, 0, 1));

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
		Render(0, SCRWIDTH);
	}
}

void RayTracer::Render(unsigned int xStart, unsigned int xEnd)
{
	for (unsigned int i = xStart; i < xEnd; ++i) for (unsigned int j = 0; j < SCRHEIGHT; ++j)
	{
		if (msaaStatus == MSAA::NONE)
		{
			Ray ray = GetUVRay(uv[i][j]);
			Color col = Trace(ray);
			renderBuffer[i][j] = col.GetRGBValue();
			continue;
		}

		Ray ray = GetUVRay(uv[i][j]);
		Ray ray1 = GetUVRay(uv[i][j]);
		Ray ray2 = GetUVRay(uv[i][j]);
		Ray ray3 = GetUVRay(uv[i][j]);

		Color col = Trace(ray).GetClamped();
		Color col1 = Trace(ray1).GetClamped();
		Color col2 = Trace(ray2).GetClamped();
		Color col3 = Trace(ray3).GetClamped();

		renderBuffer[i][j] = ((col + col1 + col2 + col3) * .25).GetRGBValue();
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

		Color C = intersection.mat.GetColor(intersection.uv, intersection.position).value * environment.value;

		return C;

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

bool RayTracer::RayIsBlocked(Ray& ray, float d2, LightSource* l) const
{
	bool isDirectional = typeid(*l) == typeid(DirectionalLight);
	bool isSpotLight = typeid(*l) == typeid(SpotLight);

	for (Intersectable* object : scene.GetObjects())
	{
		Intersection i = object->Intersect(ray);

		if (isDirectional)
		{
			if (i.t > 0 && i.intersect)
				return true;
		}

		else if (i.t > 0 && i.t * i.t < d2)
		{
			return true;
		}
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

Ray RayTracer::GetUVRay(const float2& uv) const
{
	float xOffset = 0;
	float yOffset = 0;

	if (msaaStatus == MSAA::MSAA_4X) 
	{
		xOffset = RandomFloat() * uvX;
		yOffset = RandomFloat() * uvY;
	}

	Ray ray = Ray(cam.pos, normalize((cam.p0 + (uv.x + xOffset) * (cam.p1 - cam.p0) + (uv.y + yOffset) * (cam.p2 - cam.p0)) - cam.pos), 1, AIR, 0, 100);

	// Distortion happening
	float n1 = 1;
	float n2 = 1.3f;

	float indexRatio = n1 / n2; //bepaalt de graad v. barrel distortion
	float3 D = ray.Dir * -1;
	float cosi = dot(cam.viewDir, D);
	float k = 1 - ((indexRatio * indexRatio) * (1 - cosi * cosi));
	cosi = fabsf(cosi);

	float3 dir = indexRatio * ray.Dir + cam.viewDir * (indexRatio * cosi - sqrt(k));
	
	ray.Dir = dir;
	//ray.Origin = float3(cam.p0 + (uv.x + xOffset) * (cam.p1 - cam.p0) + (uv.y + yOffset) * (cam.p2 - cam.p0));

	return ray;
}