#pragma once
#include <Intersectable.h>


class BVH
{
private:
	uInt poolPtr;
	struct BVHNode
	{
		AABB bounds;
		bool isLeaf;
		BVHNode* left, * right;
		uint32_t first;
		uint32_t count;

		void Subdivide();
	};

private:
	Triangle* primitives;
	uInt n;
	uInt* indices;
	BVHNode* pool;

	BVHNode* root;
public:
	BVH(Triangle* intersectables, uint32_t count);
	void ConstructBVH();
	AABB CalculateBounds(Triangle* const primitives, uint32_t first, uint32_t count);
	void SubdivideBVHNode(BVHNode* node);
	bool Partition(BVHNode* node);
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
