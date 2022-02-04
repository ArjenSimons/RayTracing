#pragma once
class LowResTracer : public RayTracer
{
protected:
	uint pixel_size = 10;
	Color Trace(Ray& ray, uint bounceDepth = 0) override;
	void Render(Color renderBuffer[SCRWIDTH][SCRHEIGHT], unsigned int yStart, unsigned int yEnd) override;
public:
	inline LowResTracer(Scene* scene, Camera* camera) : RayTracer(scene, camera) {}
};

