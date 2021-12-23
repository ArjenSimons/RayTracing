#include "precomp.h"
#include "TopLevelBVH.h"

TopLevelBVH::TopLevelBVH(vector<BVH*>* BVHs)
	: BVHs(BVHs)
{
	float3 minBound = float3(std::numeric_limits<float>::max(), std::numeric_limits<float>::max(), std::numeric_limits<float>::max());
	float3 maxBound = float3(-std::numeric_limits<float>::max(), -std::numeric_limits<float>::max(), -std::numeric_limits<float>::max());

	for (BVH* bvh : (*BVHs))
	{
		AABB aabb = bvh->GetRoot().bounds;
		minBound.x = min(minBound.x, aabb.bmin3.x);
		minBound.y = min(minBound.y, aabb.bmin3.y);
		minBound.z = min(minBound.z, aabb.bmin3.z);
		maxBound.x = max(maxBound.x, aabb.bmax3.x);
		maxBound.y = max(maxBound.y, aabb.bmax3.y);
		maxBound.z = max(maxBound.z, aabb.bmax3.z);
	}

	bounds = AABB(minBound, maxBound);
}

//TODO: Construct tree for top levelBVH
Intersection TopLevelBVH::Traverse(Ray& r)
{
	Intersection closest_intersection;
	for (size_t i = 0; i < BVHs->size(); i++)
	{
		r.Transform((*BVHs)[i]->GetInvTranslation());
		Intersection intersection = (*BVHs)[i]->Traverse(r);
		r.Transform((*BVHs)[i]->GetTranslation());

		if (intersection.intersect && intersection.t != 0 && intersection.t < closest_intersection.t)
		{
			closest_intersection = intersection;
		}
	}
	return closest_intersection;
}

