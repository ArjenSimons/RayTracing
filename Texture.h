#pragma once
#include "RayTracing.h"

class Texture
{
public:
	virtual Color value(float2 uv, const float3& p) const = 0;
};

class ColorTexture : public Texture
{
public:
	ColorTexture(Color color) : color(color) {};

	virtual Color value(float2 uv, const float3& p) const override
	{
		return color;
	}
private:
	Color color;
};

class CheckerTexture : public Texture
{
public:
	CheckerTexture(shared_ptr<Texture> texture_even, shared_ptr<Texture> texture_odd)
		: even(texture_even), odd(texture_odd)
	{
	}

	virtual Color value(float2 uv, const float3& p) const override
	{
		float sines = sin(10 * p.x) * sin(10 * p.y) * sin(10 * p.z);

		if (sines < 0) 
		{
			return odd->value(uv, p);
		}
		else 
		{
			return even->value(uv, p);
		}
	}
private:
	shared_ptr<Texture> even;
	shared_ptr<Texture> odd;
};

class ImageTexture : public Texture
{

};
