#pragma once

class Scene
{
private: 
	vector<Intersectable*> objects;
	vector<LightSource*> lightSources;
	TopLevelBVH* topBVH = nullptr;
	function<void(float)> animations;
public:

	Scene(vector<Intersectable*> objects, vector<LightSource*> lightSources) {
		this->objects = objects;
		this->lightSources = lightSources;
	};
	Scene(vector<Intersectable*> objects, vector<LightSource*> lightSources, TopLevelBVH* topBVH) 
		: Scene(objects, lightSources) 
	{
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

	inline void AddAreaLight(Intersectable* object) {
		objects.push_back(object);
	}

	inline bool Animate(float deltaTime) {
		int i = 0;
		// Check if the std::function exists using its implicit bool conversion.
		if (animations) {
			animations(deltaTime); 
			return true;
		}
		return false;
	}

	// Traverse the BVH, then traverse the extra primitives in the scene.
	Intersection Intersect(Ray& r) {
		Intersection closest_intersection;

		if (topBVH != nullptr) {
			closest_intersection = topBVH->Traverse(r);
		}

		for (Intersectable * geometry : objects)
		{
			Intersection intersection = geometry->Intersect(r);

			if (intersection.intersect && intersection.t != 0 && intersection.t < closest_intersection.t)
			{
				closest_intersection = intersection;
			}
		}
		return closest_intersection;
	}

	inline vector<Intersectable*> GetObjects() const { return objects; }
	inline vector<LightSource*> GetLights() const { return lightSources; };
	inline TopLevelBVH* GetBVH() const { return topBVH; }
};
