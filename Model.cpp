#include "precomp.h"
#include "Intersectable.h"

Model::Model(float3 position, shared_ptr<Mesh> mesh, Substance substance, Material mat)
	: Intersectable(position, substance, mat)
{
	std::vector<float3> verticess = mesh->GetVertices();
	for (int i = 0; i < verticess.size(); i += 3)
	{
		triangles.push_back(Triangle(position + verticess[i], position + verticess[i + 1], position + verticess[i + 2], substance, mat));
	}

	printf("Model face count: %i", triangles.size());
}

Intersection Model::Intersect(Ray ray)
{
	Intersection closest_intersection;

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