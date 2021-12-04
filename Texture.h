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
public: 
	const static int BYTES_PER_PIXEL = 3;
	ImageTexture(const char* filename)
	{
		auto componentsPerPixel = BYTES_PER_PIXEL;

		imageData = stbi_load(filename, &width, &height, &componentsPerPixel, componentsPerPixel);

		bytesPerScanLine = BYTES_PER_PIXEL * width;
	}

	~ImageTexture()
	{
		delete imageData;
	}

	virtual Color value(float2 uv, const float3& p) const override
	{
		if (imageData == nullptr) return Color(1, 0, 1);

		uv.x = clamp(uv.x, 0.0f, 1.0f);
		uv.y = 1.0f - clamp(uv.y, 0.0f, 1.0f);

		int i = static_cast<int>(uv.x * width);
		int j = static_cast<int>(uv.y * height);

		if (i >= width) i = width - 1;
		if (j >= height) j = height - 1;

		unsigned char* pixel = imageData + j * bytesPerScanLine + i * BYTES_PER_PIXEL;

		return Color(colorScale * pixel[0], colorScale * pixel[1], colorScale * pixel[2]);
	}
private:
	unsigned char* imageData;
	int width, height;
	int bytesPerScanLine;
	const float colorScale = 1.0 / 255.0;
};
