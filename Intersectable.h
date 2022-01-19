#pragma once
#include "RayTracing.h"
#include "Material.h"
#include "Mesh.h"

struct Intersection
{
	bool intersect;
	float t;
	float3 position;
	float3 normal;
	Material mat{ float3(1, 0, 1), nullptr };
	Substance sTo;
	float2 uv;

	Intersection()
	{
		intersect = false;
		t = INT_MAX;
		position = float3(0, 0, 0);
		normal = float3(0, 0, 0);
	}
};

inline bool RayAABBIntersect(const Ray& ray, const AABB& aabb)
{
	float3 invDir = 1 / ray.Dir;

	float tx1 = (aabb.Minimum(0) - ray.Origin.x) * invDir.x;
	float tx2 = (aabb.Maximum(0) - ray.Origin.x) * invDir.x;

	float tmin = min(tx1, tx2);
	float tmax = max(tx1, tx2);

	float ty1 = (aabb.Minimum(1) - ray.Origin.y) * invDir.y;
	float ty2 = (aabb.Maximum(1) - ray.Origin.y) * invDir.y;

	tmin = max(tmin, min(ty1, ty2));
	tmax = min(tmax, max(ty1, ty2));

	float tz1 = (aabb.Minimum(2) - ray.Origin.z) * invDir.z;
	float tz2 = (aabb.Maximum(2) - ray.Origin.z) * invDir.z;

	tmin = max(tmin, min(tz1, tz2));
	tmax = min(tmax, max(tz1, tz2));

	return tmax >= max(0.0f, tmin);
}

class Intersectable
{
protected: 
	AABB aabb;
	float3 position;
	Substance substance;
	Material mat;
public:
	Intersectable(float3 position, Substance substance, Material mat) : position(position), substance(substance), mat(mat) {};
	virtual ~Intersectable() noexcept = default;
	virtual Intersection Intersect(Ray ray) = 0;
	float3 GetPosition() { return position; }
};

class Plane : public Intersectable
{
private:
	float3 normal;
public:
	Plane(float3 position, float3 normal, Substance substance, Material mat);
	~Plane();
	Intersection Intersect(Ray ray) override;
protected:
	float2 GetUV(float3& normal, float3& position);
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

class Torus : public Intersectable
{
private:
	float R; // Major radius
	float r; // Minor radius
	float R2; // Major radius squared
	float r2; // Minor radius squared
	mat4 transf;
	
public:
	Torus(float3 position, float minR, float majR, float3 eulerAngles, Substance substance, Material mat);
	~Torus();
	Intersection Intersect(Ray ray) override;
private:
	float GetIntersectionDistance(Ray ray);
	float3 GetNormal(float3 position);
	float2 GetUV(float3 position);
};


class Triangle : public Intersectable
{
private:
	float3 position2;
	float3 position3;
	float3 normal;
	float3 inormal;
	float3 centroid;
	AABB aabb;
public:
	Triangle(float3 p1, float3 p2, float3 p3, Substance substance, Material mat);
	~Triangle() = default;
	Intersection Intersect(Ray ray) override;
	AABB GetAABB() const { return aabb; }
	float3 GetCentroid() const { return centroid; }
private:
	void CalculateBoundingBox();
};

class Model : public Intersectable
{
private:
	std::vector<Triangle> triangles;
	AABB aabb;
	mat4 translation;
public:
	Model(float3 position, float scale, shared_ptr<Mesh> mesh, Substance substance, Material mat);
	~Model() = default;
	Intersection Intersect(Ray ray) override;
	std::vector<Triangle>* GetTriangles() { return &triangles; }
	mat4 GetTranslation() const { return translation; }
};
