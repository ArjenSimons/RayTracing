#pragma once
#include <Intersectable.h>


class BVH
{
private:
	uInt poolPtr;
	struct BVHNode
	{
		AABB bounds;
		int32_t leftFirst;
		int32_t count;

		void Subdivide();
	};

private:
	Triangle* primitives;
	uInt n;
	uInt* indices;
	BVHNode* pool;

	BVHNode* root;
public:
	BVH(Triangle* intersectables, uInt count);
	void ConstructBVH();
	AABB CalculateBounds(Triangle* const primitives, uInt first, uInt count);
	void SubdivideBVHNode(uInt ptr);
	void Partition(uInt ptr);
};

//void BVH::BVHNode::Subdivide()
//{
//	if (count < 3) return;
//	leftFirst = poolPtr++;
//	right = new BVHNode();
//	left->Subdivide();
//	right->Subdivide();
//	isLeaf = false;
//}
