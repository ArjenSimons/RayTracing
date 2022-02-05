#pragma once
// Diffuse util from Raynetracing Renderobert, tutorial from Scratchapixel
// https://www.scratchapixel.com/lessons/3d-basic-rendering/global-illumination-path-tracing/global-illumination-path-tracing-practical-implementation

inline void createCoordinateSystem(const float3& N, float3& Nt, float3& Nb)
{
	if (fabs(N.x) > fabs(N.y))
		Nt = float3(N.z, 0.f, -N.x) / sqrtf(N.x * N.x + N.z * N.z);
	else
		Nt = float3(0.f, -N.z, N.y) / sqrtf(N.y * N.y + N.z * N.z);
	Nb = cross(N, Nt);
}
inline float3 uniformSampleHemisphere(const float& r1, const float& r2)
{
	// We already know one axis.
	// cos(theta) = r1 = y
	// cos^2(theta) + sin^2(theta) = 1 -> sin(theta) = srtf(1 - cos^2(theta))
	float sinTheta = sqrtf(1 - r1 * r1);
	float phi = 2 * PI * r2;
	float x = sinTheta * cosf(phi);
	float z = sinTheta * sinf(phi);

	return float3(x, r1, z);
}

inline float3 diffuseReflection(float3 normal) {
	// Create a new coordinate system where the up axis is the normal.
	float3 Nt, Nb;
	createCoordinateSystem(normal, Nt, Nb);

	float r1 = RandomFloat();
	float r2 = RandomFloat();

	float3 random_dir = uniformSampleHemisphere(r1, r2);

	float x = random_dir.x * Nb.x + random_dir.y * normal.x + random_dir.z * Nt.x;
	float y = random_dir.x * Nb.y + random_dir.y * normal.y + random_dir.z * Nt.y;
	float z = random_dir.x * Nb.z + random_dir.y * normal.z + random_dir.z * Nt.z;
	float3 sampleWorld = float3(x, y, z);

	return sampleWorld;
}
