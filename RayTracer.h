#pragma once
#include "Scene.h"

class RayTracer
{
private:
	Scene m_scene;
public:
	RayTracer();
	~RayTracer();

	void SetScene(Scene scene);

};


