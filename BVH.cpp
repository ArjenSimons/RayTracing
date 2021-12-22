#include "precomp.h"
#include "BVH.h"

BVH::BVH(vector<Triangle>* intersectables, uInt count, bool diagnostics)
	:primitives(intersectables), n(count), diagnostics(diagnostics)
{
}

void BVH::ConstructBVH()
{

	indices = new uInt[n];
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

Intersection BVH::Traverse(Ray& r)
{
	return TraverseInner(r, root);
}

Intersection BVH::TraverseInner(Ray& r, BVHNode* node)
{
	if (!RayAABBIntersect(r, node->bounds)) return dummyIntersection;
	if (node->isLeaf)
	{
		return GetClosestIntersectionInNode(r, node);
	}
	else
	{
		Intersection left = TraverseInner(r, node->left);
		Intersection right = TraverseInner(r, node->right);

		if (left.t <= right.t) { return left; }
		else { return right; }
	}
}

Intersection BVH::GetClosestIntersectionInNode(Ray& r, BVHNode* node)
{
	Intersection closest_intersection;

	for (int i = node->first; i < node->first + node->count; i++)
	{
		Intersection intersection;
		intersection = (*primitives)[indices[i]].Intersect(r);

		if (intersection.intersect && (closest_intersection.intersect == false || intersection.t < closest_intersection.t))
		{
			closest_intersection = intersection;
		}
	}
	return closest_intersection;
}

AABB BVH::CalculateBounds(uint32_t first, uint32_t count)
{
	//Make variables;
	float3 minBound = minb;
	float3 maxBound = maxb;

	AABB aabb;
	for (int i = first; i < first + count; i++)
	{
		AABB aabb = (*primitives)[indices[i]].GetAABB();
		minBound.x = min(minBound.x, aabb.bmin3.x);
		minBound.y = min(minBound.y, aabb.bmin3.y);
		minBound.z = min(minBound.z, aabb.bmin3.z);
		maxBound.x = max(maxBound.x, aabb.bmax3.x);
		maxBound.y = max(maxBound.y, aabb.bmax3.y);
		maxBound.z = max(maxBound.z, aabb.bmax3.z);
		//aabb.Grow((*primitives)[indices[i]].GetAABB());
	}

	return AABB(minBound, maxBound);
}

void BVH::SubdivideBVHNode(BVHNode* node)
{
	if (node->count < 3)
	{
		node->isLeaf = true;
		return;
	}

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

	int splitAxis = 0;
	AABB aabb;
	float lowestCost = minb.x;
	float bestBinPos;
	float axisWidths[3] = { node->bounds.bmax3.x - node->bounds.bmin3.x, node->bounds.bmax3.y - node->bounds.bmin3.y, node->bounds.bmax3.z - node->bounds.bmin3.z };
	float binPositions[3] = { node->bounds.bmin3.x, node->bounds.bmin3.y, node->bounds.bmin3.z };
	//Binning
	for (int k = 0; k < 3; k++)
	{
		float binDist = axisWidths[k] * iBinCount;

		for (int j = 0; j < binCount - 1; j++)
		{
			binPositions[k] += binDist;
			AABB left, right;
			float3 leftMinBound = minb;
			float3 leftMaxBound = maxb;
			float3 rightMinBound = minb;
			float3 rightMaxBound = maxb;
			int leftCount = 0;
			int rightCount = 0;
			for (int i = node->first; i < node->first + node->count; i++)
			{
				float p = 0;
				switch (k)
				{
				case(0):
					p = (*primitives)[indices[i]].GetCentroid().x;
					break;
				case(1):
					p = (*primitives)[indices[i]].GetCentroid().y;
					break;
				case(2):
					p = (*primitives)[indices[i]].GetCentroid().z;
					break;
				}

				AABB aabb = (*primitives)[indices[i]].GetAABB();
				if (p < binPositions[k])
				{
					//left.Grow((*primitives)[indices[i]].GetCentroid());
					leftMinBound.x = min(leftMinBound.x, aabb.bmin3.x);
					leftMinBound.y = min(leftMinBound.y, aabb.bmin3.y);
					leftMinBound.z = min(leftMinBound.z, aabb.bmin3.z);
					leftMaxBound.x = max(leftMaxBound.x, aabb.bmax3.x);
					leftMaxBound.y = max(leftMaxBound.y, aabb.bmax3.y);
					leftMaxBound.z = max(leftMaxBound.z, aabb.bmax3.z);
					leftCount++;
				}
				else
				{
					//right.Grow((*primitives)[indices[i]].GetCentroid());
					rightMinBound.x = min(rightMinBound.x, aabb.bmin3.x);
					rightMinBound.y = min(rightMinBound.y, aabb.bmin3.y);
					rightMinBound.z = min(rightMinBound.z, aabb.bmin3.z);
					rightMaxBound.x = max(rightMaxBound.x, aabb.bmax3.x);
					rightMaxBound.y = max(rightMaxBound.y, aabb.bmax3.y);
					rightMaxBound.z = max(rightMaxBound.z, aabb.bmax3.z);
					rightCount++;
				}
			}

			left = AABB(leftMinBound, leftMaxBound);
			right = AABB(rightMinBound, rightMaxBound);
			//SAH
			float leftArea = left.Area();
			float rightArea = right.Area();
			leftArea = isinf(leftArea) ? 0 : leftArea;
			rightArea = isinf(rightArea) ? 0 : rightArea;
			float cost = leftArea * leftCount + rightArea * rightCount;

			if (cost < lowestCost)
			{
				lowestCost = cost;
				bestBinPos = binPositions[k];
				splitAxis = k;
			}
		}
	}

	float parentArea = node->bounds.Area();
	float parentCost = parentArea * node->count;
	//printf("parentCost: %f\n", parentCost);
	//printf("parentArea: %f\n", parentArea);
	if (lowestCost >= parentCost)
	{
		node->left = nullptr;
		node->right = nullptr;
		node->isLeaf = true;
		return false;
	}

	int j = node->first - 1;

	switch (splitAxis)
	{
	case(0):
		for (int i = node->first; i < node->first + node->count; i++)
		{
			if ((*primitives)[indices[i]].GetCentroid().x < bestBinPos)
			{
				std::swap(indices[++j], indices[i]);
			}
		}

		break;
	case(1):
		for (int i = node->first; i < node->first + node->count; i++)
		{
			if ((*primitives)[indices[i]].GetCentroid().y < bestBinPos)
			{
				std::swap(indices[++j], indices[i]);
			}
		}
		break;
	case(2):
		for (int i = node->first; i < node->first + node->count; i++)
		{
			if ((*primitives)[indices[i]].GetCentroid().z < bestBinPos)
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
}

int BVH::countNodes(const BVHNode& node) const
{
	if (node.isLeaf) return 1;

	return 1 + countNodes(*node.left) + countNodes(*node.right);
}