struct Ray
{
	float3 Origin;
	float3 Dir;
	float t;

	Ray(float3 origin = float3(0, 0, 0), float3 dir = float3(0, 1, 0), float t = 1)
		: Origin(origin), Dir(dir), t(t)
	{
	}
};