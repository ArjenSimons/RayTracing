#pragma once
enum Substance
{
	SOLID,
	AIR,
	WATER,
	GLASS,
	DIAMOND
};

struct Ray
{
	float3 Origin;
	float3 Dir;
	Substance substance;
	float e;
	unsigned int nBounces = 0;
	float t;

	Ray(float3 origin = float3(0, 0, 0), float3 dir = float3(0, 1, 0), float e = 1, Substance substance = AIR, unsigned int nBounces = 0, float t = 1)
		: Origin(origin), Dir(dir), e(e), substance(substance), t(t)
	{
	}
};

inline float RefractionIndex(Substance substance)
{
	switch (substance)
	{
	case SOLID:
		return 0;
		break;
	case AIR:
		return 1.0f;
		break;
	case WATER:
		return 1.333f;
		break;
	case GLASS:
		return 1.52f;
		break;
	case DIAMOND:
		return 2.417f;
	default:
		break;
	}
}