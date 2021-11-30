#include "precomp.h"
#include "Intersectable.h"

Sphere::Sphere(float3 position, float radius, Substance substance, Material mat)
	: Intersectable(position, substance, mat), radius2(radius * radius)
{

}

Sphere::~Sphere()
{

}

Intersection Sphere::Intersect(Ray ray)
{
	Intersection out;

	float3 C = position - ray.Origin;
	float t = dot(C, ray.Dir);
	float3 Q = C - t * ray.Dir;
	float p2 = dot(Q, Q);

	if (p2 > radius2) { return out; } //No hit

	t -= sqrt(radius2 - p2);
	if ((t > 0))
	{
		out.t = t;
		out.intersect = true;
		out.position = (ray.Origin + out.t * ray.Dir);
		out.normal = normalize(out.position - position);
		out.mat = mat;
		//printf("intersect pos: %f, %f, %f \n", out.position.x, out.position.y, out.position.z);
	}

	return out;
}