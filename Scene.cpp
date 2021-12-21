#include "precomp.h"
#include "Scene.h"

Scene::Scene()
{
	printf("creating scene\n");
}

Scene::~Scene() 
{
	
}

void Scene::AddBVH(BVH* bvhTree)
{
	bvh = bvhTree;
}

void Scene::AddObject(Intersectable* object) 
{
	objects.push_back(object);
}

void Scene::AddLightSource(LightSource* lightSource)
{
	lightSources.push_back(lightSource);
}