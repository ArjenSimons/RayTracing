#include "precomp.h"
#include "Intersectable.h"

Model::Model(float3 position, float scale, shared_ptr<Mesh> mesh, Substance substance, Material mat)
	: Intersectable(position, substance, mat)
{
	std::vector<float3> verticess = mesh->GetVertices();

	float3 minBound = float3(std::numeric_limits<float>::max(), std::numeric_limits<float>::max(), std::numeric_limits<float>::max());
	float3 maxBound = float3(-std::numeric_limits<float>::max(), -std::numeric_limits<float>::max(), -std::numeric_limits<float>::max());
	printf("minfloat: %f\n", std::numeric_limits<float>::min());

	for (int i = 0; i < verticess.size(); i += 3)
	{
		float3 vertices[3] = { position + verticess[i] * scale, position + verticess[i + 1] * scale, position + verticess[i + 2] * scale };

		triangles.push_back(Triangle(vertices[0], vertices[1], vertices[2], substance, mat));

		for (float3 v : vertices)
		{
			minBound.x = min(minBound.x, v.x);
			minBound.y = min(minBound.y, v.y);
			minBound.z = min(minBound.z, v.z);
			maxBound.x = max(maxBound.x, v.x);
			maxBound.y = max(maxBound.y, v.y);
			maxBound.z = max(maxBound.z, v.z);
		}
	}

	aabb = AABB(minBound, maxBound);

	//printf("Model face count: %i\n", triangles.size());
	//printf("min: %f, %f, %f\n", minBound.x, minBound.y, minBound.z);
	//printf("max: %f, %f, %f\n", maxBound.x, maxBound.y, maxBound.z);
}

Intersection Model::Intersect(Ray ray)
{
	Intersection closest_intersection;

	if (!RayAABBIntersect(ray, aabb)) { return closest_intersection; }

	for (Triangle triangle : triangles)
	{
		Intersection i = triangle.Intersect(ray);

		if (i.intersect && (closest_intersection.intersect == false || i.t < closest_intersection.t))
		{
			closest_intersection = i;
		}
	}

	return closest_intersection;
}