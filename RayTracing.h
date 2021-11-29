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