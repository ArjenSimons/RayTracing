#pragma once

class Scene
{
private: 
	vector<Intersectable*> objects;
	vector<LightSource*> lightSources;
	TopLevelBVH* topBVH;
	function<void(float)> animations;
public:

	Scene(vector<Intersectable*> objects, vector<LightSource*> lightSources, TopLevelBVH* topBVH) {
		this->objects = objects;
		this->lightSources = lightSources;
		this->topBVH = topBVH;
	};
	Scene() {}
	~Scene() {}

	inline void AddBVH(TopLevelBVH* bvhTree) {
		topBVH = bvhTree; 
	}

	inline void AddObject(Intersectable* object) {
		objects.push_back(object); 
	}

	inline void AddLightSource(LightSource* lightSource) { 
		lightSources.push_back(lightSource); 
	}

	inline void AddAnimations(function<void(float)> animations) { 
		this->animations = animations; 
	}

	inline void Animate(float deltaTime) {
		int i = 0;
		// Check if the std::function exists using its implicit bool conversion.
		if (animations) {
			animations(deltaTime); 
		}
	}

	inline vector<Intersectable*> GetObjects() const { return objects; }
	inline vector<LightSource*> GetLights() const { return lightSources; };
	inline TopLevelBVH* GetBVH() const { return topBVH; }
};
