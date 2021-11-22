#pragma once
#include "RayTracing.h"

class Intersectable
{
protected: 
	float3 m_position;
public:
	Intersectable(float3 position) : m_position(position) {};
	virtual ~Intersectable() noexcept = default;
	virtual bool Intersect(Ray ray) = 0;
};