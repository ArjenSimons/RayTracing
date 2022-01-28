#pragma once

inline float3 Lerp(float3 p1, float3 p2, float alpha)
{
	return p1 * (1 - alpha) + p2 * alpha;
}