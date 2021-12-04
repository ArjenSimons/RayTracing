#pragma once
#include "Texture.h"

class Material
{
protected:
	Color color;
	shared_ptr<Texture> texture;
public:
	float specularity;
public:
	Material(Color color, shared_ptr<Texture> texture, float specularity = 0);
	~Material();
	Color GetColor(float2 uv, float3 position) const;
};