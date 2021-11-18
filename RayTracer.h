#pragma once
class RayTracer
{
};

struct Ray
{
	float3 Origin;
	float3 Dir;

	Ray(
		float3 origin = float3(0, 0, 0),
		float3 dir = float3(0, 0, 0)
	): Origin(origin), Dir(dir)
	{
	}
};
