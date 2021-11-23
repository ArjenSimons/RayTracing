#include "precomp.h"
#include "Sphere.h"

Sphere::Sphere(float3 position, float radius)
	: Intersectable(position), m_radius(radius)
{

}

Sphere::~Sphere()
{

}

Intersection Sphere::Intersect(Ray ray) const 
{
	Intersection out;



	return out;
}