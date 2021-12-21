#pragma once
#include "Intersectable.h"
#include "LightSource.h"
#include "BVH.h"

class Scene
{
private: 
	std::vector<Intersectable*> objects;
	std::vector<LightSource*> lightSources;
	BVH* bvh;
public:

	Scene();
	~Scene();

	void AddBVH(BVH* bvh);
	void AddObject(Intersectable* object);
	void AddLightSource(LightSource* lightSource);
	std::vector<Intersectable*> GetObjects() const { return objects; }
	std::vector<LightSource*> GetLights() const { return lightSources; };
	BVH* GetBVH() const { return bvh; }
};
