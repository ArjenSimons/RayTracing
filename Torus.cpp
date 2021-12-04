#include "precomp.h"
#include "Intersectable.h"
#include "algebra.h"

Torus::Torus(float3 position, float minorR, float majorR, Substance substance, Material mat)
	: Intersectable(position, substance, mat), r(minorR), R(majorR)
{
	r2 = r * r;
	R2 = R * R;
}

Torus::~Torus()
{

}

Intersection Torus::Intersect(Ray ray)
{
	Intersection out;

	float t = GetIntersectionDistance(ray);
	if (t != -1) // this means we have found an intersection
	{

		out.t = t;
		if (out.t > 0)
		{
			out.intersect = true;
			out.position = ray.Origin + ray.Dir * t;
			out.normal = GetNormal(out.position);
			out.mat = mat;
			out.sTo = substance;
			out.uv = GetUV(out.position);
		}
	}

	return out;
}

float Torus::GetIntersectionDistance(Ray ray)
{
	float3 o = ray.Origin;
	float3 d = ray.Dir;

	float A, B, C, D, E, F;
	A = 4 * R2 * (d.x * d.x + d.y * d.y);
	B = 8 * R2 * (o.x * d.x + o.y * d.y);
	C = 4 * R2 * (o.x * o.x + o.y * o.y);
	D = d.x * d.x + d.y * d.y + d.z * d.z;
	E = 2 * dot(o, d);
	F = o.x * o.x + o.y * o.y + o.z * o.z + R2 - r2;

	double a, b, c, dd, e;
	a = D * D;
	b = 2 * D * E;
	c = 2 * D * F + E * E - A * A;
	dd = 2 * E * F - B;
	e = F * F - C;

	double res[4];
	int resAmt = Algebra::SolveQuarticEquation(a, b, c, dd, e, res);

	std::vector<float> positives;
	for (int i = 0; i < resAmt; i++)
	{
		if (res[i] > 0)
			positives.push_back((float)res[i]);
	}

	if (positives.empty())
		return -1;

	std::sort(positives.begin(), positives.end());
	return positives[0];
}

float3 Torus::GetNormal(float3 position)
{
	float3 Pprime = float3(position.x, position.y, 0);
	float3 Q = normalize(Pprime) * sqrt(R2);
	float3 N = position - Q;
	return normalize(N);
}

float2 Torus::GetUV(float3 position)
{
	float x = position.x;
	float y = position.y;
	float z = position.z;

	float u = 0.5f + atan2(z, x) / (2 * PI);
	float v = 0.5f + atan2(y, (sqrt(x * x + z * z) - sqrt(R2))) / (2 * PI);

	return float2(u, v);
}