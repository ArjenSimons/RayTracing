#include "precomp.h"
#include "Plane.h"

Plane::Plane(float3 position, float3 normal) 
	: Intersectable(position), m_normal(normal)
{

}

Plane::~Plane() 
{

}

bool Plane::Intersect(Ray ray)
{
	return false;
}