#pragma once

class Scene
{
private: 
	std::vector<Intersectable*> objects;
	std::vector<LightSource*> lightSources;
	TopLevelBVH* topBVH;
public:

	Scene();
	~Scene();

	void AddBVH(TopLevelBVH* bvh);
	void AddObject(Intersectable* object);
	void AddLightSource(LightSource* lightSource);
	std::vector<Intersectable*> GetObjects() const { return objects; }
	std::vector<LightSource*> GetLights() const { return lightSources; };
	TopLevelBVH* GetBVH() const { return topBVH; }
};
