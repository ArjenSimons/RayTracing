#include "precomp.h"
#include "Intersectable.h"

Plane::Plane(float3 position, float3 normal, Material mat) 
	: Intersectable(position, mat), normal(normal)
{
	
}

Plane::~Plane()
{

}

Intersection Plane::Intersect(Ray ray)
{
	Intersection out;

	float denom = dot(normal, ray.Dir);

	if (denom != 0)
	{
		out.t = (dot((position - ray.Origin), normal)) / denom;

		if (out.t > 0)
		{
			out.intersect = true;
			out.position = ray.Origin + out.t * ray.Dir;
			out.normal = normal;
			out.mat = mat;
		}		
	}

	return out;
}