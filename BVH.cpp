#include "precomp.h"
#include "BVH.h"

BVH::BVH(Triangle* intersectables, uInt count)
	:primitives(intersectables), n(count)
{
}

void BVH::ConstructBVH()
{
	indices = new uInt[n];
	printf("n = %i \n", n);
	for (int i = 0; i < n; i++) indices[i] = i;

	pool = new BVHNode[n * 2];
	root = &pool[0];
	poolPtr = 2;

	//subdivide node
	root->leftFirst = 0;
	root->count = n;
	root->bounds = CalculateBounds(primitives, root->leftFirst, root->count);
	SubdivideBVHNode(0);
}

AABB BVH::CalculateBounds(Triangle* const primitives, uInt first, uInt count)
{
	//Make variables;
	float3 minBound = float3(std::numeric_limits<float>::max(), std::numeric_limits<float>::max(), std::numeric_limits<float>::max());
	float3 maxBound = float3(-std::numeric_limits<float>::max(), -std::numeric_limits<float>::max(), -std::numeric_limits<float>::max());

	for (int i = first; i < first + count; i++)
	{
		//float3 pos = primitives[i].GetCentroid();
		AABB* aabb = &primitives[i].GetAABB();

		minBound.x = min(minBound.x, aabb->bmin3.x);
		minBound.y = min(minBound.y, aabb->bmin3.y);
		minBound.z = min(minBound.z, aabb->bmin3.z);
		maxBound.x = max(maxBound.x, aabb->bmax3.x);
		maxBound.y = max(maxBound.y, aabb->bmax3.y);
		maxBound.z = max(maxBound.z, aabb->bmax3.z);
	}

	return AABB(minBound, maxBound);
}

void BVH::SubdivideBVHNode(uInt ptr)
{
	if (pool[ptr].count < 3) return;
	pool[ptr].leftFirst = poolPtr++;
	poolPtr++;

	Partition(ptr);

	SubdivideBVHNode(pool[ptr].leftFirst);
	SubdivideBVHNode(pool[ptr].leftFirst + 1);
}

void BVH::Partition(uInt ptr)
{
	//Determine split axis
	//Determine split position
	float splitPos = pool[ptr].bounds.Center(pool[ptr].bounds.LongestAxis());
	AABB aabb = pool[0].bounds;

	int j = ptr - 1;
	int count = pool[ptr].count;

	switch (pool[ptr].bounds.LongestAxis())
	{
	case(0):
		for (int i = ptr; i < ptr + pool[ptr].count; i++)
		{
			if (primitives[indices[i]].GetCentroid().x < splitPos)
			{
				std::swap(indices[++j], indices[i]);
			}

			//uInt* secondHalf = std::partition(&indices[pool[ptr].leftFirst], &indices[pool[ptr].leftFirst + pool[ptr].count],
			//	[splitPos](const Intersectable& i) {
			//	return i.GetPosition().x < splitPos;
			//});
		}

		break;
	case(1):
		for (int i = ptr; i < ptr + pool[ptr].count; i++)
		{
			if (primitives[indices[i]].GetCentroid().y < splitPos)
			{
				std::swap(indices[++j], indices[i]);
			}
		}
		break;
	case(2):
		for (int i = ptr; i < ptr + pool[ptr].count; i++)
		{
			if (primitives[indices[i]].GetCentroid().z < splitPos)
			{
				std::swap(indices[++j], indices[i]);
			}
		}
		break;
	}


	//Left
	int left = pool[ptr].leftFirst;
	pool[left].count = j - ptr + 1;
	pool[left].bounds = CalculateBounds(primitives, left, pool[left].count);
	//Right
	int right = pool[ptr].leftFirst + 1;
	pool[right].count = ptr - pool[left].count;
	pool[right].bounds = CalculateBounds(primitives, left + pool[left].count - 1, pool[right].count - 1);
}