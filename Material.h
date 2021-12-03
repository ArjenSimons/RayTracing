#pragma once
#include "Texture.h"

enum MatType {
	UNIFORM,
	CHECKER
};



class Material
{
protected:
	Color color;
	Color secondColor;
	shared_ptr<Texture> texture;
	MatType type;
public:
	float specularity;
public:
	Material(Color color, shared_ptr<Texture> texture, float specularity = 0, Color secondColor = float3(1, 0, 1), MatType type = UNIFORM);
	~Material();
	Color GetColor(float2 uv, float3 position) const;
};