#pragma once
#include "Material.h"
class CheckerMaterial : public Material
{
private:
	float3 secondColor;
public:
	CheckerMaterial(float3 color, float3 secondColor);
	~CheckerMaterial();
	float3 GetColor(float3 position);
};

