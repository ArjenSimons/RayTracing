#include "precomp.h"
#include "BVH.h"

BVH::BVH(vector<Triangle> intersectables, uInt count, bool diagnostics)
	:primitives(intersectables), n(count), diagnostics(diagnostics)
{
	printf("p: %i", intersectables);
}

void BVH::ConstructBVH()
{
	printf("p: %i", primitives);

	indices = new uInt[n];
	printf("n = %i \n", n);
	for (int i = 0; i < n; i++) indices[i] = i;

	pool = new BVHNode[n * 2];
	root = &pool[0];
	poolPtr = 2;

	//subdivide node
	root->first = 0;
	root->count = n;
	root->bounds = CalculateBounds(root->first, root->count);
	SubdivideBVHNode(root);

	if (diagnostics)
	{
		printf("Number of nodes: %i\n", countNodes(*root));
	}
}

AABB BVH::CalculateBounds(uint32_t first, uint32_t count)
{
	//Make variables;
	float3 minBound = float3(std::numeric_limits<float>::max(), std::numeric_limits<float>::max(), std::numeric_limits<float>::max());
	float3 maxBound = float3(-std::numeric_limits<float>::max(), -std::numeric_limits<float>::max(), -std::numeric_limits<float>::max());
	AABB aabb = primitives[6316].GetAABB();
	//printf("\n min: %f %f %f\n", minBound.x, minBound.y, minBound.z);
	//printf("max: %f %f %f\n", maxBound.x, maxBound.y, maxBound.z);
	//printf("\nFirst+count= %i", first + count);
	//printf("\nFirst= %i", first);
	for (int i = 0; i < n; i++)
	{

		AABB aabb = primitives[i].GetAABB();
		minBound.x = min(minBound.x, aabb.bmin3.x);
		minBound.y = min(minBound.y, aabb.bmin3.y);
		minBound.z = min(minBound.z, aabb.bmin3.z);
		maxBound.x = max(maxBound.x, aabb.bmax3.x);
		maxBound.y = max(maxBound.y, aabb.bmax3.y);
		maxBound.z = max(maxBound.z, aabb.bmax3.z);

	}

	//for (int i = 0; i < n; i++)
	//{

	//		printf("yes");
	//	//float3 pos = primitives[i].GetCentroid();
	//	AABB aabb = primitives[i].GetAABB();

	//	minBound.x = min(minBound.x, aabb.bmin3.x);
	//	minBound.y = min(minBound.y, aabb.bmin3.y);
	//	minBound.z = min(minBound.z, aabb.bmin3.z);
	//	maxBound.x = max(maxBound.x, aabb.bmax3.x);
	//	maxBound.y = max(maxBound.y, aabb.bmax3.y);
	//	maxBound.z = max(maxBound.z, aabb.bmax3.z);

	//	//printf("\n min: %f %f %f\n", minBound.x, minBound.y, minBound.z);
	//	//printf("max: %f %f %f\n", maxBound.x, maxBound.y, maxBound.z);
	//}
	//printf("\n min: %f %f %f\n", minBound.x, minBound.y, minBound.z);
	//printf("max: %f %f %f\n", maxBound.x, maxBound.y, maxBound.z);
	return AABB(minBound, maxBound);
}

void BVH::SubdivideBVHNode(BVHNode* node)
{
	if (node->count < 3)
	{
		node->isLeaf = true;
		return;
	}
	printf("yeet\n");
	node->isLeaf = false;
	node->left = &pool[poolPtr++];
	node->right = &pool[poolPtr++];

	if (Partition(node))
	{
		SubdivideBVHNode(node->left);
		SubdivideBVHNode(node->right);
	}
}

bool BVH::Partition(BVHNode* node)
{
	//Determine split axis
	//Determine split position
	float splitPos = node->bounds.Center(node->bounds.LongestAxis());
	AABB aabb = node->bounds;

	//printf("\nsplit %f\n", splitPos);
	//printf("node %i", node);
	//printf("Bound %f\n", node->bounds.bmin3.z);

	int j = node->first - 1;
	int count = node->count;
	//printf("count %i\n", count);

	int con = node->first + count;

	switch (node->bounds.LongestAxis())
	{
	case(0):
		for (int i = node->first; i < con; i++)
		{
			float x = primitives[indices[i]].GetCentroid().x;
			//printf("x %f", x);
			if (x < splitPos)
			{
				std::swap(indices[++j], indices[i]);
			}
		}

		break;
	case(1):
		for (int i = node->first; i < con; i++)
		{
			float y = primitives[indices[i]].GetCentroid().y;
			//printf("y %f", y);
			if (primitives[indices[i]].GetCentroid().y < splitPos)
			{
				std::swap(indices[++j], indices[i]);
			}
		}
		break;
	case(2):
		for (int i = node->first; i < con; i++)
		{
			float z = primitives[indices[i]].GetCentroid().z;
			//printf("z %f", z);

			if (z < splitPos)
			{
				std::swap(indices[++j], indices[i]);
			}
		}
		break;
	}

	BVHNode* left = node->left;
	left->count = j - node->first + 1;
	left->first = node->first;
	left->bounds = CalculateBounds(left->first, left->count);

	BVHNode* right = node->right;
	right->count = node->count - left->count;
	right->first = left->first + left->count;
	right->bounds = CalculateBounds(right->first, right->count);

	if (left->count == node->count || right->count == node->count) {
		node->left = nullptr;
		node->right = nullptr;
		node->isLeaf = true;
		return false;
	}

	return true;
	////Left
	//int left = pool[ptr].leftFirst;
	//int c1 = j - ptr + 1;
	//pool[left].count = j - ptr + 1;
	//printf("c1 %i:\n", pool[left].count);
	//printf("ptr %i:\n", ptr);
	//pool[left].bounds = CalculateBounds(primitives, left, pool[left].count);
	////Right
	//int right = ptr + 1;
	//int c = pool[ptr].count - pool[left].count;
	//int start = left + pool[left].count;
	//pool[right].count = c;
	//pool[right].bounds = CalculateBounds(primitives, start, pool[right].count - 1);

}

int BVH::countNodes(const BVHNode& node) const
{
	if (node.isLeaf) return 1;

	return 1 + countNodes(*node.left) + countNodes(*node.right);
}