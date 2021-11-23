#include "precomp.h"
#include "Plane.h"

Plane::Plane(float3 position, float3 normal) 
	: Intersectable(position), m_normal(normal)
{
	printf("hallo");
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

		if (out.t >= 0 && out.t < ray.t)
		{
			out.intersect = true;
			out.position = ray.Origin + out.t * ray.Dir;
		}		
	}

	return out;
}