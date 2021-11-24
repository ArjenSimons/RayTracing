#pragma once
#include "RayTracing.h"
#include "Material.h"

struct Intersection
{
	bool intersect;
	float t;
	float3 position;
	float3 normal;
	Material mat{ float3(1, 0, 1) };

	Intersection()
	{
		intersect = false;
		t = INT_MAX;
		position = float3(0, 0, 0);
		normal = float3(0, 0, 0);
	}
};

class Intersectable
{
protected: 
	float3 m_position;
	Material m_mat;
public:
	Intersectable(float3 position, Material mat) : m_position(position), m_mat(mat) {};
	virtual ~Intersectable() noexcept = default;
	virtual Intersection Intersect(Ray ray) const = 0;
};

class Plane : public Intersectable
{
private:
	float3 m_normal;
public:
	Plane(float3 position, float3 normal, Material mat);
	~Plane();
	Intersection Intersect(Ray ray) const override;
};

class Sphere : public Intersectable
{
private:
	float m_radius2;
public:
	Sphere(float3 position, float radius, Material mat);
	~Sphere();
	Intersection Intersect(Ray ray) const override;
};

