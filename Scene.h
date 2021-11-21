#pragma once
#include "Intersectable.h"

class Scene
{
private: 
	std::vector<Intersectable*> m_objects;
public:
	std::vector<Intersectable*> GetObjects() const { return m_objects; };
};

