#pragma once
#include "RayTracing.h"
struct Intersection
{
	bool intersect;
	float t;
	float3 position;

	Intersection() 
	{
		intersect = false;
		t = INT_MAX;
		position = float3(0, 0, 0);
	}
};

class Intersectable
{
protected: 
	float3 m_position;
public:
	Intersectable(float3 position) : m_position(position) {};
	virtual ~Intersectable() noexcept = default;
	virtual Intersection Intersect(Ray ray) const = 0;
};

