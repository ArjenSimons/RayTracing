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

		float phi = atan2(out.normal.z, out.normal.x) + PI;
		float theta = acos(-out.normal.y);
		out.uv = float2(phi / (2 * PI), theta / PI);
	}
}

Intersection Sphere::InsideIntersect(Ray ray)
{
	Intersection out;

	float3 oc = ray.Origin - position;
	float a = dot(ray.Dir, ray.Dir);
	float b = 2.0f * dot(oc, ray.Dir);
	float c = dot(oc, oc) - radius2;
	float d = b * b - 4 * a * c;

	float t = (-b + sqrtf(d)) / (2.0f * a);


	if ((t >= 0))
	{
		out.t = t;
		out.intersect = true;
		out.position = (ray.Origin + out.t * ray.Dir);
		float3 normal = normalize(out.position - position);
		out.normal = normal * -1;
		out.mat = mat;
		out.sTo = AIR;

		float phi = atan2(-normal.z, normal.x) + PI;
		float theta = acos(-normal.y);
		out.uv = float2(phi / (2 * PI), theta / PI);
	}

	return out;
}
