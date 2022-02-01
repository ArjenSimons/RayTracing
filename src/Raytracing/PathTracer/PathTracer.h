#pragma once
class PathTracer: public RayTracer
{
private:
	Accumulator accumulator;
protected:
	Color Trace(Ray& ray, unsigned int bounceDepth = 0) override;
	void Render(Color renderBuffer[SCRWIDTH][SCRHEIGHT], unsigned int yStart, unsigned int yEnd) override;
public:
	inline PathTracer(Scene* scene, Camera* camera) : RayTracer(scene, camera), accumulator() {}
	inline void OnCameraUpdate() override { accumulator.resetAccumulator(); };
	void Render(Color renderBuffer[SCRWIDTH][SCRHEIGHT]) override;
};