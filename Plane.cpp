#include "precomp.h"
#include "Plane.h"

Plane::Plane(float3 position, float3 normal) 
	: Intersectable(position), m_normal(normal)
{

}

Plane::~Plane() 
{

}

Intersection Plane::Intersect(Ray ray) const
{
	Intersection out;

	float denom = dot(m_normal, ray.Dir);

	if (denom != 0) 
	{
		out.t = -(dot((m_position - ray.Origin), m_normal)) / denom;

		out.intersect = out.t >= 0;
		out.position = ray.Origin + out.t * ray.Dir;
	}
	else 
	{
		out.intersect = false;
	}

	return out;
}