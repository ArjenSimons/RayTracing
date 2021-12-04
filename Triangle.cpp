#include "precomp.h"
#include "Intersectable.h"

Triangle::Triangle(float3 p1, float3 p2, float3 p3, Substance substance, Material mat)
	: Intersectable(p1, substance, mat), position2(p2), position3(p3)
{
	float3 a = position2 - position;
	float3 b = position3 - position;

	normal = cross(a, b);
}

Intersection Triangle::Intersect(Ray ray)
{
	Intersection out;

	float denom = dot(ray.Dir, normal);

	if (denom != 0)
	{
		float d = -dot(normal, position);
		float t = -(dot(normal, ray.Origin) + d) / denom;

		if (t > 0) // ray is in quad
		{
			float3 p = ray.Origin + t * ray.Dir;

			float3 C;

			float3 edge0 = position2 - position;
			float3 p0 = p - position;
			C = cross(edge0, p0);
			if (dot(normal, C) < 0) { return out; } //ray is on right of edge

			float3 edge1 = position3 - position2;
			float3 p1 = p - position2;
			C = cross(edge1, p1);
			if (dot(normal, C) < 0) { return out; } //ray is on right of edge

			float3 edge2 = position - position3;
			float3 p2 = p - position3;
			C = cross(edge2, p2);
			if (dot(normal, C) < 0) { return out; } //ray is on right of edge

			out.t = t;
			out.intersect = true;
			out.position = p;
			out.normal = normal;
			out.mat = mat;
			out.sTo = substance;
			//out.uv = GetUV(normal, out.position);
		}
	}

	return out;
}