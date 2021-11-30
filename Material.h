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
	Material(Color color, Color secondColor = float3(1, 0, 1), MatType type = SOLID);
	~Material();
	Color GetColor(float3 position);
};