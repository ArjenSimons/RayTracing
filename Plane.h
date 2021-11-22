#pragma once
#include "Intersectable.h"
class Plane : public Intersectable
{
private:
	float3 m_normal;
public:
	Plane(float3 position, float3 normal);
	~Plane();
	bool Intersect(Ray ray) override;
};

