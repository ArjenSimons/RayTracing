#pragma once

struct LightSource
{
	float3 position;
	float intensity;
	Color color;

	LightSource(float3 position, float intensity, Color color)
		: position(position), intensity(intensity), color(color)
	{

	}

	virtual float3 GetDir(float3 objPos) = 0;
	virtual bool IsBlocked(float3 rayDirection) { return false; }
};

struct PointLight : LightSource
{
	PointLight(float3 position, float intensity, Color color)
		: LightSource(position, intensity, color)
	{ }

	float3 GetDir(float3 objPos) override
	{
		return position - objPos;
	}
};

struct DirectionalLight : LightSource
{
	float3 dir;

	DirectionalLight(float3 position, float3 direction, float intensity, Color color)
		: LightSource(position, intensity, color), dir(normalize(direction))
	{ }

	float3 GetDir(float3 point) override
	{
		return dir;
	}
};

struct SpotLight : LightSource
{
	float3 dir;
	float a;

	SpotLight(float3 position, float3 direction, float angle, float intensity, Color color)
		: LightSource(position, intensity, color), dir(-1*direction), a(angle * PI / 180)
	{ }

	float3 GetDir(float3 objPos) override
	{
		return position - objPos;
	}

	bool IsBlocked(float3 rayDirection) override
	{
		float incidentAngle = acos(dot(dir, rayDirection));
		return incidentAngle > a;
	}
};