#include "precomp.h"
#include "Sphere.h"

Sphere::Sphere(float3 position, float radius)
	: Intersectable(position), m_radius2(radius * radius)
{
	printf("sphere r2 = %f \n", m_radius2);
}

Sphere::~Sphere()
{

}

Intersection Sphere::Intersect(Ray ray) const 
{
	Intersection out;

	float3 C = m_position - ray.Origin;
	float t = dot(C, ray.Dir);
	float3 Q = C - t * ray.Dir;
	float p2 = dot(Q, Q);

	if (p2 > m_radius2) { return out; } //No hit

	t -= sqrt(m_radius2 - p2);
	if ((t >= 0 && t < ray.t)) 
	{
		out.t, ray.t = t;
		out.intersect = true;
		out.position = ray.Origin + out.t * ray.Dir;
	}

	return out;
}