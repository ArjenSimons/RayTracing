#pragma once
#include "Material.h"

struct LightSource
{
	float3 position;
	float intensity;
	Color color;

	LightSource(float3 position, float intensity, Color color)
		: position(position), intensity(intensity), color(color)
	{

	}
};
