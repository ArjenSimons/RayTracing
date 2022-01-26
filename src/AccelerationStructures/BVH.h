#pragma once

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

	mat4 translation;
	mat4 invTranslation;
	
	Intersection dummyIntersection;

	bool diagnostics;
	float3 minb = float3(std::numeric_limits<float>::max(), std::numeric_limits<float>::max(), std::numeric_limits<float>::max());
	float3 maxb = float3(-std::numeric_limits<float>::max(), -std::numeric_limits<float>::max(), -std::numeric_limits<float>::max());

	//1.0e-5 for optimal sbvh, 1 for full bvh
	float spatialSplitConstraint = 1;

public:
	BVH(vector<Triangle>* intersectables, uint32_t count, mat4 translation, bool diagnostics);
	void ConstructBVH();
	Intersection Traverse(Ray& r);
	BVHNode GetRoot() { return *root; }
	vector<Triangle>* GetPrims() { return primitives; }
	mat4 GetTranslation() { return translation; }
	mat4 GetInvTranslation() { return invTranslation; }
	void Translate(float3 translation);
	void RotateX(float rotation);
	void RotateY(float rotation);
	void RotateZ(float rotation);
private:
	Intersection TraverseInner(Ray& r, BVHNode* node, uint& nChecks);
	Intersection GetClosestIntersectionInNode(Ray& r, BVHNode* node, uint& nChecks);
	AABB CalculateBounds(uint32_t first, uint32_t count);
	void SubdivideBVHNode(BVHNode* node);
	bool Partition(BVHNode* node);
	int countNodes(const BVHNode& node) const;
};

class BVHInstance
{
private:
	BVH* bvh;
	mat4 translation;
	mat4 invTranslation;
public:
	BVHInstance(BVH* bvh)
		: bvh(bvh)
	{
		translation = bvh->GetTranslation();
		invTranslation = translation.Inverted();
	}
	BVH* GetBVH() { return bvh; }
	mat4 GetTranslation() { return translation; }
	mat4 GetInvTranslation() { return invTranslation; }
	void Translate(float3 t)
	{
		//TODO: Probably a better way to do this
		translation.Translate(t);

		float3 tr = translation.GetTranslation();
		tr += t;
		translation = mat4::Translate(tr);
		invTranslation = translation.Inverted();
	}
	void RotateX(float r) { mat4 rot = mat4::RotateX(r); translation = translation * rot; invTranslation = translation.Inverted(); }
	void RotateY(float r) { mat4 rot = mat4::RotateY(r); translation = translation * rot; invTranslation = translation.Inverted(); }
	void RotateZ(float r) { mat4 rot = mat4::RotateZ(r); translation = translation * rot; invTranslation = translation.Inverted(); }
};