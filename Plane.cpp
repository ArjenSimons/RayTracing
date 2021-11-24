#include "precomp.h"
#include "Intersectable.h"

Plane::Plane(float3 position, float3 normal, Material mat) 
	: Intersectable(position, mat), m_normal(normal)
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
		//out.t = -(dot(ray.Origin, m_normal) + 0) / denom;

		if (out.t <= 0)
		{
			out.intersect = true;
			out.position = ray.Origin + out.t * ray.Dir;
			out.normal = m_normal;
			out.mat = m_mat;
		}		
	}

	return out;
}