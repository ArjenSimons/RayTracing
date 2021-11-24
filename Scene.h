#pragma once
#include "Intersectable.h"
#include "LightSource.h"

class Scene
{
private: 
	std::vector<Intersectable*> m_objects;
	std::vector<LightSource*> m_lightSources;
public:
	Scene();
	~Scene();

	void AddObject(Intersectable* object);
	void AddLightSource(LightSource* lightSource);
	std::vector<Intersectable*> GetObjects() const { return m_objects; }
	std::vector<LightSource*> GetLights() const { return m_lightSources; };
};
