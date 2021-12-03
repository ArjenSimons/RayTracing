#pragma once
#include "Intersectable.h"
#include "LightSource.h"

class Scene
{
private: 
	std::vector<Intersectable*> objects;
	std::vector<LightSource*> lightSources;
public:

	Scene();
	~Scene();

	void AddObject(Intersectable* object);
	void AddLightSource(LightSource* lightSource);
	std::vector<Intersectable*> GetObjects() const { return objects; }
	std::vector<LightSource*> GetLights() const { return lightSources; };
};
