#pragma once

class WhittedRayTracer: public RayTracer
{
protected:
	Color Trace(Ray& ray, unsigned int bounceDepth = 0) override;
	void Render(Color renderBuffer[SCRWIDTH][SCRHEIGHT], unsigned int yStart, unsigned int yEnd);
public:
	WhittedRayTracer(Scene* scene, Camera* cam) : RayTracer(scene, cam) {};
	~WhittedRayTracer() {};
private:
	Color DirectIllumination(float3 point, float3 normal);
	Color TraceDielectrics(const Ray& ray, const Intersection& intersection, unsigned int bounceDepth);
	bool RayIsBlocked(Ray& ray, float d2, LightSource* l) const;
};
