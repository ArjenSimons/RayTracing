struct Ray
{
	float3 Origin;
	float3 Dir;
	unsigned int nBounces = 0;
	float e;
	float t;

	Ray(float3 origin = float3(0, 0, 0), float3 dir = float3(0, 1, 0), float e = 1, unsigned int nBounces = 0, float t = 1)
		: Origin(origin), Dir(dir), e(e), t(t)
	{
	}
};

enum Substance
{
	SOLID,
	AIR,
	WATER,
	GLASS
};

float RefractionIndex(Substance substance)
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
	default:
		break;
	}
}