#pragma once
enum MatType {
	SOLID,
	CHECKER
};

struct Color
{
	float3 value;

	Color(float3 color)
		: value(color)
	{
	}

	unsigned int GetRGBValue() 
	{
		//TODO: Clamp value to 1 before mulitplying
		return ((int)(value.x * 255) << 16) + ((int)(value.y * 255) << 8) + ((int)(value.z * 255));
	}
};

class Material
{
protected:
	Color color;
	Color secondColor;
	MatType type;
public:
	float specularity;
public:
	Material(Color color, Color secondColor = float3(1, 0, 1), float specularity = 0, MatType type = SOLID);
	~Material();
	Color GetColor(float3 position);
};