#pragma once
#include "BVH.h"
#include "Intersectable.h"

class TopLevelBVH
{
	//struct TopBVHNode
	//{
	//	AABB bounds;
	//	TopBVHNode* left, * right;
	//	BVH* bvh;
	//	bool isLeaf;

	//	TopBVHNode(TopBVHNode* left, TopBVHNode* right, BVH* bvh)
	//		: left(left), right(right)
	//	{}
	//};

private:
	vector<BVH*>* BVHs;

	AABB bounds;
public:
	TopLevelBVH(vector<BVH*>* BVHs);
	Intersection Traverse(Ray& r);
};

