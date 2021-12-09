#include "precomp.h"
#include "Intersectable.h"

Triangle::Triangle(float3 p1, float3 p2, float3 p3, Substance substance, Material mat)
	: Intersectable(p1, substance, mat), position2(p2), position3(p3)
{
	float3 a = position2 - position;
	float3 b = position3 - position;

	normal = normalize(cross(a, b));
	inormal = -normal;
}

Intersection Triangle::Intersect(Ray ray)
{
	Intersection out;

	float3 p1p2 = position2 - position;
	float3 p1p3 = position3 - position;
	float3 pvec = cross(ray.Dir, p1p3);
	float det = dot(pvec, p1p2);

	if (det == 0) return out; //miss

	float invDet = 1 / det;

	float3 tvec = ray.Origin - position;
	float v = dot(tvec, pvec) * invDet;
	if (v < 0 || v > 1) return out; //miss

	float3 qvec = cross(tvec, p1p2);
	float u = dot(ray.Dir, qvec) * invDet;
	if (u < 0 || u + v > 1) return out; //miss

	float t = dot(p1p3, qvec) * invDet;

	if (t > 000.1) 
	{
		out.t = t;
		out.intersect = true;
		out.position = ray.Origin + t * ray.Dir;
		out.normal = det < 0 ? inormal : normal;
		out.mat = mat;
		out.sTo = substance;
		out.uv = float2(u,  v);
	}

	return out;
}