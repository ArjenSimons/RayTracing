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
	Substance sTo;

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
	float3 position;
	Substance substance;
	Material mat;
public:
	Intersectable(float3 position, Substance substance, Material mat) : position(position), substance(substance), mat(mat) {};
	virtual ~Intersectable() noexcept = default;
	virtual Intersection Intersect(Ray ray) = 0;
};

class Plane : public Intersectable
{
private:
	float3 normal;
public:
	Plane(float3 position, float3 normal, Substance substance, Material mat);
	~Plane();
	Intersection Intersect(Ray ray) override;
};

class Sphere : public Intersectable
{
private:
	float radius2;
public:
	Sphere(float3 position, float radius, Substance substance, Material mat);
	~Sphere();
	Intersection Intersect(Ray ray) override;
private:
	Intersection OutsideIntersect(Ray ray);
	Intersection InsideIntersect(Ray ray);
};

