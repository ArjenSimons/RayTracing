#pragma once
#include "Intersectable.h"
#include "CheckerMaterial.h"

class Plane : public Intersectable
{
private:
	float3 m_normal;
public:
	Plane(float3 position, float3 normal, Material mat);
	Plane(float3 position, float3 normal, CheckerMaterial mat);
	~Plane();
	Intersection Intersect(Ray ray) const override;
};
