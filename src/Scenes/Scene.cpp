#include "precomp.h"

Scene::Scene()
{
}

Scene::~Scene() 
{
	
}

void Scene::AddBVH(TopLevelBVH* bvhTree)
{
	topBVH = bvhTree;
}

void Scene::AddObject(Intersectable* object) 
{
	objects.push_back(object);
}

void Scene::AddLightSource(LightSource* lightSource)
{
	lightSources.push_back(lightSource);
}