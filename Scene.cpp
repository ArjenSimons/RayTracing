#include "precomp.h"
#include "Scene.h"

Scene::Scene() 
{

}

Scene::~Scene() 
{

}

void Scene::AddObject(Intersectable* object) 
{
	m_objects.push_back(object);
}