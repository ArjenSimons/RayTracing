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
	if (ray.substance == substance) 
	{
		return InsideIntersect(ray);
	}
	else
	{
		return OutsideIntersect(ray);
	}
}

//TODO: Fix duplicate code
Intersection Sphere::OutsideIntersect(Ray ray)
{
	Intersection out;

	float3 C = position - ray.Origin;
	float t = dot(C, ray.Dir);
	float3 Q = C - t * ray.Dir;
	float p2 = dot(Q, Q);

	if (p2 > radius2) { return out; } //No hit

	t -= sqrt(radius2 - p2);
	if ((t >= 0))
	{
		out.t = t;
		out.intersect = true;
		out.position = (ray.Origin + out.t * ray.Dir);
		out.normal = normalize(out.position - position);
		out.mat = mat;
		out.sTo = substance;
		out.sFrom = ray.substance;
	}
}

Intersection Sphere::InsideIntersect(Ray ray)
{
	Intersection out;

	float3 oc = ray.Origin - position;
	float a = dot(ray.Dir, ray.Dir);
	float b = 2.0 * dot(oc, ray.Dir);
	float c = dot(oc, oc) - radius2;
	float d = b * b - 4 * a * c;

	float t = (-b + sqrtf(d)) / (2.0 * a);


	if ((t >= 0))
	{
		out.t = t;
		out.intersect = true;
		out.position = (ray.Origin + out.t * ray.Dir);
		out.normal = normalize((out.position - position) * -1);
		out.mat = mat;
		out.sTo = AIR;
		out.sFrom = ray.substance;
	}

	return out;
}
