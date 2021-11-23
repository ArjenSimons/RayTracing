#pragma once
#include "Intersectable.h"
class Sphere : public Intersectable
{
private:
	float m_radius2;
public:
	Sphere(float3 position, float radius);
	~Sphere();
	Intersection Intersect(Ray ray) const override;
};

