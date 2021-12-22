#pragma once
#include <Intersectable.h>


class BVH
{
private:
	struct BVHNode
	{
		AABB bounds;
		bool isLeaf;
		BVHNode* left, * right;
		uint32_t first;
		uint32_t count;
	};

private:
	vector<Triangle>* primitives;
	uInt n;
	uInt* indices;
	uInt poolPtr;

	int binCount = 4;
	float iBinCount = 1.0f / binCount;
	BVHNode* pool;
	BVHNode* root;

	Intersection dummyIntersection;

	bool diagnostics;
	float3 minb = float3(std::numeric_limits<float>::max(), std::numeric_limits<float>::max(), std::numeric_limits<float>::max());
	float3 maxb = float3(-std::numeric_limits<float>::max(), -std::numeric_limits<float>::max(), -std::numeric_limits<float>::max());
public:
	BVH(vector<Triangle>* intersectables, uint32_t count, bool diagnostics);
	void ConstructBVH();
	Intersection Traverse(Ray& r);
	BVHNode GetRoot() { return *root; }
	vector<Triangle>* GetPrims() { return primitives; }
private:
	Intersection TraverseInner(Ray& r, BVHNode* node);
	Intersection GetClosestIntersectionInNode(Ray& r, BVHNode* node);
	AABB CalculateBounds(uint32_t first, uint32_t count);
	void SubdivideBVHNode(BVHNode* node);
	bool Partition(BVHNode* node);
	int countNodes(const BVHNode& node) const;
};
