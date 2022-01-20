#include "precomp.h"
//#include "Intersectable.h"

Plane::Plane(float3 position, float3 normal, Substance substance, Material mat) 
	: Intersectable(position, substance, mat), normal(normal)
{
	
}

Plane::~Plane()
{

}

Intersection Plane::Intersect(Ray ray)
{
	Intersection out;

	float denom = dot(normal, ray.Dir);

	if (denom != 0)
	{
		out.t = (dot((position - ray.Origin), normal)) / denom;

		if (out.t > 0)
		{
			out.intersect = true;
			out.position = ray.Origin + out.t * ray.Dir;
			out.normal = denom < 0 ? normal : normal * -1;
			out.mat = mat;
			out.sTo = substance;
			out.uv = GetUV(normal, out.position);
		}		
	}

	return out;
}

float2 Plane::GetUV(float3& N, float3& p)
{
	float3 a = cross(N, float3(1, 0, 0));
	float3 b = cross(N, float3(0, 1, 0));
	float3 c = cross(N, float3(0, 0, 1));

	float3 maxAB = dot(a, a) < dot(b, b) ? b : a;

	float3 vAxis = normalize(dot(maxAB, maxAB) < dot(c, c) ? c : maxAB);
	float3 uAxis = cross(N, vAxis);

	float vt = dot(vAxis, p * .5);
	float ut = dot(uAxis, p * .5);

	float u = ut > 0 ? abs(ut) - abs(floor(ut)) : abs(ut) - abs(ceil(ut));
	float v = vt > 0 ? abs(vt) - abs(floor(vt)) : abs(vt) - abs(ceil(vt));

	return float2(u, v);
}