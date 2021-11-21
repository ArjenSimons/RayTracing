#pragma once
#include "RayTracing.h"

class Intersectable
{
private: 
	float3 position;
public:
	Intersectable() = default;
	virtual ~Intersectable() noexcept = default;
	virtual void Intersect(Ray ray) = 0;
};