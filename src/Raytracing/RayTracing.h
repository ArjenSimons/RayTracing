#pragma once

const unsigned int processor_count = std::thread::hardware_concurrency();

enum Substance
{
	SOLID,
	AIR,
	WATER,
	GLASS,
	ABSORBING_GLASS,
	DIAMOND
};

enum ThreadingStatus
{
	THREADING_DISABLED,
	THREADING_ENABLED
};

enum class MSAA
{
	NONE,
	MSAA_4X
};

struct Ray
{
	float3 Origin;
	float3 Dir;
	Substance substance;
	float e;
	unsigned int nBounces = 0;
	float t;

	Ray(float3 origin = float3(0, 0, 0), float3 dir = float3(0, 1, 0), float e = 1, Substance substance = AIR, unsigned int nBounces = 0, float t = 1)
		: Origin(origin), Dir(dir), e(e), substance(substance), t(t)
	{
	}

	void Transform(mat4 mat)
	{
		Origin = mat.TransformPoint(Origin);
		Dir = mat.TransformVector(Dir);
	}
};

inline float RefractionIndex(Substance substance)
{
	switch (substance)
	{
	case SOLID:
		return 0;
		break;
	case AIR:
		return 1.0f;
		break;
	case WATER:
		return 1.333f;
		break;
	case GLASS:
		return 1.52f;
		break;
	case ABSORBING_GLASS:
		return 1.52f;
		break;
	case DIAMOND:
		return 2.417f;
	default:
		break;
	}
}

inline float Absorption(Substance substance) 
{
	switch (substance)
	{
	case SOLID:
		return .0f;
		break;
	case AIR:
		return .0f;
		break;
	case WATER:
		return .1f;
		break;
	case GLASS:
		return .02f;
		break;
	case ABSORBING_GLASS:
		return .5f;
		break;
	case DIAMOND:
		return .02f;
	default:
		break;
	}
}

// TODO: Replace this struct by making color an alias of float3. Or just leave colors as float3 values.
struct Color
{
	float3 value;

	Color()
		: value(float3(0, 0, 0))
	{
	}

	Color(float3 color)
		: value(color)
	{
	}

	Color(float r, float g, float b)
		: value(float3(r, g, b))
	{
	}

	Color GetClamped()
	{
		return Color(clamp(value.x, 0.0, 1.0), clamp(value.y, 0.0, 1.0), clamp(value.z, 0.0, 1.0));
	}

	unsigned int GetRGBValue()
	{
		return ((int)(clamp(value.x, 0.0, 1.0) * 255) << 16) + ((int)(clamp(value.y, 0.0, 1.0) * 255) << 8) + ((int)(clamp(value.z, 0.0, 1.0) * 255));
	}

	bool operator==(const Color& rhs) const
	{
		return (value.x == rhs.value.x && value.y == rhs.value.y && value.z == rhs.value.z);
	}

	Color operator+(const Color& rhs) const
	{
		return float3(value.x + rhs.value.x, value.y + rhs.value.y, value.z + rhs.value.z);
	}

	Color operator-(const Color& rhs) const
	{
		return float3(value.x - rhs.value.x, value.y - rhs.value.y, value.z - rhs.value.z);
	}

	Color operator*(const Color& rhs) const
	{
		return float3(value.x * rhs.value.x, value.y * rhs.value.y, value.z * rhs.value.z);
	}

	Color operator*(const float& rhs) const
	{
		return float3(value.x * rhs, value.y * rhs, value.z * rhs);
	}

	friend inline void operator+=(Color& a, Color b) { a.value += b.value; }
};