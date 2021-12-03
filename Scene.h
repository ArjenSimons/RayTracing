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


//static Scene GetTelescopeScene() 
//{
//	Scene scene = Scene();
//	Plane* plane = new Plane(float3(0, -1, 0), float3(0, 1, 0), SOLID, Material(float3(1, 1, 1), 0, float3(0, 0, 0), CHECKER));
//	Sphere* sphere = new Sphere(float3(0, 0, 2), 1, GLASS, Material(float3(1, 1, 1), 0));
//	Sphere* sphere1 = new Sphere(float3(0, 0, 5), .2, GLASS, Material(float3(1, 1, 1), 0));
//	Sphere* sphere2 = new Sphere(float3(0, 0, 5.35), .1, SOLID, Material(float3(1, 0, 0), 0));
//
//	LightSource* light = new LightSource(float3(1, 1, 3), 10, float3(1, 1, 1));
//	LightSource* light1 = new LightSource(float3(-1, 3, -1), 10, float3(1, 1, 1));
//	LightSource* light2 = new LightSource(float3(0, 0, 3.1), 10, float3(1, 1, 1));
//
//	
//	LightSource* light3 = new LightSource(float3(0, 0, 5.21), 10, float3(1, 1, 1));
//	LightSource* light4 = new LightSource(float3(-1, 1, 5.21), 10, float3(1, 1, 1));
//	LightSource* light5 = new LightSource(float3(1, 1, 5.21), 10, float3(1, 1, 1));
//	LightSource* light6 = new LightSource(float3(-1, -.5, 5.21), 10, float3(1, 1, 1));
//	LightSource* light7 = new LightSource(float3(1, -.5, 5.21), 10, float3(1, 1, 1));
//
//	scene.AddObject(plane);
//	scene.AddObject(sphere);
//	scene.AddObject(sphere1);
//	scene.AddObject(sphere2);
//	scene.AddLightSource(light);
//	scene.AddLightSource(light1);
//	scene.AddLightSource(light2);
//	scene.AddLightSource(light3);
//	scene.AddLightSource(light4);
//	scene.AddLightSource(light5);
//	scene.AddLightSource(light6);
//	scene.AddLightSource(light7);
//
//	return scene;
//}