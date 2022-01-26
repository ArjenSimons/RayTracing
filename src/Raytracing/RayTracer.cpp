#include "precomp.h"

//RayTracer::RayTracer()
//{
//	renderBuffer = std::vector<std::vector<float3>>(SCRWIDTH, std::vector<float3>(SCRHEIGHT, float3(0, 0, 0)));
//
//	//for (int i = 0; i < SCRWIDTH; i++) for (int j = 0; j < SCRHEIGHT; j++)
//	//{
//	//	uv[i][j] = float2(static_cast<float>(i) / static_cast<float>(SCRWIDTH), static_cast<float>(j) / static_cast<float>(SCRHEIGHT));
//	//}
//}

RayTracer::RayTracer(Scene* scene, float distortion, unsigned int maxBounces, ThreadingStatus threadingStatus, MSAA msaaStatus)
	: scene(scene), df(clamp(distortion, 0.0f, 1.0f)), maxBounces(maxBounces), threadingStatus(threadingStatus), threadPool(processor_count), msaaStatus(msaaStatus)
{
	//renderBuffer = std::vector<std::vector<float3>>(SCRWIDTH, std::vector<float3>(SCRHEIGHT, float3(0, 0, 0)));
	cam = Camera();

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

void RayTracer::SetScene(Scene* scene)
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
			renderBuffer[i][j] = col;
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

		renderBuffer[i][j] = ((col + col1 + col2 + col3) * .25);
	}
}

//TODO: Compute post processing effects on GPU
void RayTracer::AddVignette(float outerRadius, float smoothness, float intensity)
{
	outerRadius = clamp(outerRadius, 0.0f, 1.0f);
	intensity = clamp(intensity, 0.0f, 1.0f);
	smoothness = max(0.0001f, smoothness);

	float innerRadius = outerRadius - smoothness;

	for (int i = 0; i < SCRWIDTH; i++) for (int j = 0; j < SCRHEIGHT; j++)
	{
		float2 p = uv[i][j] - .5f;
		float len = dot(p, p);
		float vignette = smoothstep(outerRadius, innerRadius, len) * intensity;

		renderBuffer[i][j] = renderBuffer[i][j] * vignette;
	}
}

void RayTracer::AddGammaCorrection(float gamma)
{
	float invGamma = 1 / gamma;

	for (int i = 0; i < SCRWIDTH; i++) for (int j = 0; j < SCRHEIGHT; j++)
	{
		float x = pow(renderBuffer[i][j].value.x, invGamma);
		float y = pow(renderBuffer[i][j].value.y, invGamma);
		float z = pow(renderBuffer[i][j].value.z, invGamma);

		renderBuffer[i][j] = float3(x, y, z);
	}
}

void RayTracer::AddChromaticAberration(int2 redOffset, int2 greenOffset, int2 blueOffset)
{
	for (int i = 0; i < SCRWIDTH; i++) for (int j = 0; j < SCRHEIGHT; j++)
	{
		float r = renderBuffer[i - redOffset.x][j - redOffset.y].value.x;
		float g = renderBuffer[i - greenOffset.x][j - greenOffset.y].value.y;
		float b = renderBuffer[i - blueOffset.x][j - blueOffset.y].value.z;

		tempBuffer[i][j] = float3(r, g, b);
	}

	for (int i = 0; i < SCRWIDTH; i++) for (int j = 0; j < SCRHEIGHT; j++)
	{
		renderBuffer[i][j] = tempBuffer[i][j];
	}
}

Color RayTracer::Trace(Ray& ray, unsigned int bounceDepth)
{
	float3 black(0, 0, 0);
	float3 sky(.4, .7, .8);

	Intersection intersection = GetNearestIntersection(ray);

	//Color rgb;
	//float value = static_cast<float>(intersection.nAABBandTriChecks) / 6000000.0f;

	///*if (intersection.nAABBandTriChecks > 20000000)
	//	printf("nchecks %i\n", intersection.nAABBandTriChecks);*/

	//
	//if (value > 0.5) {
	//	value -= 0.5;
	//	rgb.value.z = 0;
	//	rgb.value.y = (1.0f - 2.0f * value);
	//	rgb.value.x = (2.0f * value);
	//}
	//if (value <= 0.5) {
	//	rgb.value.z = (1.0f - 2.0f * value);
	//	rgb.value.y = (2.0f * value);
	//	rgb.value.x = 0;
	//}

	//return rgb;

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

		Color C = intersection.mat.GetColor(intersection.uv, intersection.position) * environment;

		return C;

		// -----------------------------------------------------------
		//zBuffer
		// -----------------------------------------------------------
		//return float3(1 / intersection.position.z, 1 / intersection.position.z, 1 / intersection.position.z) / 2;
	}
}

Intersection RayTracer::GetNearestIntersection(Ray& ray)
{
	return scene->GetBVH()->Traverse(ray);

	Intersection closest_intersection;

	for (Intersectable* obj : scene->GetObjects())
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
	Color out = float3(1, 1, 1);

	float cosa = clamp(dot(N, normalize(-float3(.2f, -.8f, .1f))), 0.0, 1.0);

	return out * cosa;

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

	Intersection i = scene->GetBVH()->Traverse(ray);

	if (isDirectional)
	{
		if (i.t > 0.001 && i.intersect)
			return true;
	}

	else if (i.t > 0.001 && i.t * i.t < d2)
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

Ray RayTracer::GetUVRay(const float2& uv) const
{
	float xOffset = 0;
	float yOffset = 0;

	if (msaaStatus == MSAA::MSAA_4X) 
	{
		xOffset = RandomFloat() * uvX;
		yOffset = RandomFloat() * uvY;
	}

	return Ray(cam.pos, normalize((cam.p0 + (uv.x + xOffset) * (cam.p1 - cam.p0) + (uv.y + yOffset) * (cam.p2 - cam.p0)) - cam.pos), 1, AIR, 0, 100);

	// Calculate lens distortion
	// indexRatio determines the degree of distortion
	// Because df is clamped on [0..1], 0.5 <= ratio <= 1 will always hold true
	// The same code can in theory be used to implement a pincushion distortion, if a ratio > 1 is used
	// however, our code currently does not support this.
	//float indexRatio = 1 / (1 + df); 
	//float3 D = ray.Dir * -1;
	//float cosi = dot(cam.viewDir, D);
	//float k = 1 - ((indexRatio * indexRatio) * (1 - cosi * cosi));
	//cosi = fabsf(cosi);

	//float3 dir = indexRatio * ray.Dir + cam.viewDir * (indexRatio * cosi - sqrt(k));
	//ray.Dir = normalize(dir);

	//return ray;
}
