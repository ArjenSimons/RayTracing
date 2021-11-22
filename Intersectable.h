#pragma once
#include "RayTracing.h"
struct Intersection
{
	bool intersect;
	float t;
	float3 position;
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

