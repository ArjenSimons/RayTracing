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
		return ((int)(clamp(value.x, 0.0, 1.0) * 255) << 16) + ((int)(clamp(value.y, 0.0, 1.0) * 255) << 8) + ((int)(clamp(value.z, 0.0, 1.0) * 255));
	}

	bool operator==(const Color &rhs) const
	{
		return (value.x == rhs.value.x && value.y == rhs.value.y && value.z == rhs.value.z);
	}

	Color operator+(const Color &rhs) const
	{
		return float3(value.x + rhs.value.x, value.y + rhs.value.y, value.z + rhs.value.z);
	}

	Color operator-(const Color &rhs) const
	{
		return float3(value.x - rhs.value.x, value.y - rhs.value.y, value.z - rhs.value.z);
	}

	Color operator*(const Color& rhs) const
	{
		return float3(value.x * rhs.value.x, value.y * rhs.value.y, value.z * rhs.value.z);
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
	Material(Color color, float specularity = 0, Color secondColor = float3(1, 0, 1), MatType type = SOLID);
	~Material();
	Color GetColor(float3 position);
};