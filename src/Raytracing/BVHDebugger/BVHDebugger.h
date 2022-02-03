#pragma once
class BVHDebugger: public RayTracer
{
protected:
	Color Trace(Ray& ray, unsigned int bounceDepth = 0) override;
	void Render(Color renderBuffer[SCRWIDTH][SCRHEIGHT], unsigned int yStart, unsigned int yEnd) override;
public:
	inline BVHDebugger(Scene* scene, Camera* camera) : RayTracer(scene, camera) {
		// We don't need MSAA in a debugger.
		AASamples = 1;
	}
};
