#include "precomp.h"
#include "Intersectable.h"

Sphere::Sphere(float3 position, float radius, Material mat)
	: Intersectable(position, mat), m_radius2(radius * radius)
{
	printf("sphere r2 = %f \n", m_radius2);
	printf("sphere position = %f, %f, %f \n", m_position.x, m_position.y, m_position.z);
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
	if ((t >= 0))
	{
		out.t = t;
		out.intersect = true;
		out.position = ray.Origin + out.t * ray.Dir;
		out.normal = normalize(out.position - m_position);
		out.mat = m_mat;

		//printf("intersect pos: %f, %f, %f \n", out.position.x, out.position.y, out.position.z);
	}

	return out;
}