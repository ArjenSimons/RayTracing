#include "precomp.h"
#include "Plane.h"

Plane::Plane(float3 position, float3 normal) 
	: Intersectable(position), m_normal(normal)
{

}

Plane::~Plane() 
{

}

bool Plane::Intersect(Ray ray) const
{
	float t = -(dot((m_position - ray.Origin), m_normal)) / dot(m_normal, ray.Dir);

	return t >= 0;
}