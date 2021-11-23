#pragma once
#include "Intersectable.h"
class Sphere : public Intersectable
{
private:
	float m_radius;
public:
	Sphere(float3 position, float radius);
	~Sphere();
	Intersection Intersect(Ray ray) const override;
};

