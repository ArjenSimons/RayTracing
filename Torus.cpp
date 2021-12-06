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

	float G, H, I, J, K, L;
	G = 4 * R2 * (d.x * d.x + d.y * d.y);
	H = 8 * R2 * (o.x * d.x + o.y * d.y);
	I = 4 * R2 * (o.x * o.x + o.y * o.y);
	J = dot(d, d);
	K = 2 * dot(o, d);
	L = dot(o, o) + R2 - r2;

	double a, b, c, dd, e;
	a = J * J;
	b = 2 * J * K;
	c = 2 * J * L + K * K - G;
	dd = 2 * K * L - H;
	e = L * L - I;

	double res[4];
	int resAmt = Algebra::SolveQuarticEquation(a, b, c, dd, e, res);

	std::vector<float> positives;
	for (int i = 0; i < resAmt; i++)
	{
		if (res[i] > 0.01)
			positives.push_back((float)res[i]);
	}

	if (positives.empty())
		return 0;

	std::sort(positives.begin(), positives.end());
	return positives[0];
}

float3 Torus::GetNormal(float3 position)
{
	float3 P = float3(position.x, position.y, 0);
	float3 Q = normalize(P) * R;
	float3 N = position - Q;
	return normalize(N);
}

float2 Torus::GetUV(float3 position)
{
	float x = position.x;
	float y = position.y;
	float z = position.z;

	float u = 0.5f + atan2(z, x) / (2 * PI);
	float v = 0.5f + atan2(y, (sqrt(x * x + z * z) - R)) / (2 * PI);

	return float2(u, v);
}