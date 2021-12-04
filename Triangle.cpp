#include "precomp.h"
#include "Intersectable.h"

Triangle::Triangle(float3 p1, float3 p2, float3 p3, float3 normal, Substance substance, Material mat)
	: Intersectable(p1, substance, mat), position2(p2), position3(p3), normal(normal)
{
}

Intersection Triangle::Intersect(Ray ray)
{
	Intersection out;

	return out;
}