#pragma once

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
	vector<BVHInstance*>* BVHs;

	AABB bounds;
public:
	TopLevelBVH(vector<BVHInstance*>* BVHs);
	Intersection Traverse(Ray& r);
};

