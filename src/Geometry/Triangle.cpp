#include "precomp.h"

Triangle::Triangle(float3 p1, float3 p2, float3 p3, Substance substance, Material mat)
	: Intersectable(p1, substance, mat), position2(p2), position3(p3)
{
	float3 a = position2 - position;
	float3 b = position3 - position;

	normal = normalize(cross(a, b));
	inormal = -normal;

	CalculateBoundingBox();

	centroid = float3(
		(position.x + position2.x + position3.x) / 3,
		(position.y + position2.y + position3.y) / 3,
		(position.z + position2.z + position3.z) / 3);
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
	if (t > 0.001f) 
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

void Triangle::CalculateBoundingBox() 
{
	float3 minBound;
	float3 maxBound;

	minBound.x = min(min(position.x, position2.x), position3.x);
	minBound.y = min(min(position.y, position2.y), position3.y);
	minBound.z = min(min(position.z, position2.z), position3.z);
	maxBound.x = max(max(position.x, position2.x), position3.x);
	maxBound.y = max(max(position.y, position2.y), position3.y);
	maxBound.z = max(max(position.z, position2.z), position3.z);

	aabb = AABB(minBound, maxBound);
}