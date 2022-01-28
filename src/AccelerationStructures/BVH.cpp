#include "precomp.h"

BVH::BVH(vector<Triangle>* intersectables, uInt count, mat4 translation, bool diagnostics)
	:primitives(intersectables), n(count), translation(translation), diagnostics(diagnostics)
{
	invTranslation = translation.Inverted();
}

void BVH::ConstructBVH()
{
	//Allocate extra memory for indices when constructing the sbvh
	if (spatialSplitConstraint == 1) 
		indices.resize(n); 
	else
		indices.resize(n * 1.2);


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
		printf("Number of references %i\n", root->count);
	}
}

Intersection BVH::Traverse(Ray& r)
{
	uint nChecks = 0;
	return TraverseInner(r, root, nChecks);
}

void BVH::Translate(float3 t)
{
	//TODO: Probably a better way to do this
	translation.Translate(t);

	float3 tr = translation.GetTranslation();
	tr += t;
	translation = mat4::Translate(tr);
	invTranslation = translation.Inverted();
}

void BVH::RotateX(float r)
{
	mat4 rot = mat4::RotateX(r);
	translation = translation * rot;
	invTranslation = translation.Inverted();

}
void BVH::RotateY(float r)
{
	mat4 rot = mat4::RotateY(r);
	translation = translation * rot;
	invTranslation = translation.Inverted();

}
void BVH::RotateZ(float r)
{
	mat4 rot = mat4::RotateZ(r);;
	translation = translation * rot;
	invTranslation = translation.Inverted();

}

Intersection BVH::TraverseInner(Ray& r, BVHNode* node, uint& nChecks)
{

	if (!RayAABBIntersect(r, node->bounds))
	{
		Intersection dummy;
		dummy.nAABBandTriChecks = nChecks;
		return dummy;
	}
	nChecks++;
	if (node->isLeaf)
	{
		//Intersection dummy;
		//dummy.nAABBandTriChecks = nChecks;
		////printf("nchecks: %i\n", dummy.nAABBandTriChecks);

		//return dummy;
		Intersection intersection = GetClosestIntersectionInNode(r, node, nChecks);
		return intersection;
	}
	else
	{
		//printf("oof");
		//nChecks += 2;
		Intersection left = TraverseInner(r, node->left, nChecks);
		Intersection right = TraverseInner(r, node->right, nChecks);

		if (left.t <= right.t) { return left; }
		else { return right; }
	}
}

Intersection BVH::GetClosestIntersectionInNode(Ray& r, BVHNode* node, uint& nChecks)
{
	Intersection closest_intersection;

	for (int i = node->first; i < node->first + node->count; i++)
	{
		Intersection intersection;
		intersection = (*primitives)[indices[i]].Intersect(r);

		if (intersection.intersect && (closest_intersection.intersect == false || intersection.t < closest_intersection.t))
		{
			closest_intersection = intersection;
			//printf("hit i=%i\n", i);
			nChecks++;
		}
		nChecks++;
	}
	//printf("checks %i\n", nChecks);
	//nChecks += node->first + node->count;
	closest_intersection.nAABBandTriChecks = nChecks;

	//if (!closest_intersection.intersect) {
	//	printf("ha %i", nChecks);
	//}

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
	//printf("Node sa: %f\n", node->bounds.bmin3.x);
	if (node->count == 1)
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

	AABB aabb;
	float lowestCost = minb.x;
	float bestBinPos;
	
	//Binning
	int splitAxis = node->bounds.LongestAxis();

	pair<AABB, AABB> left_right = SplitAABB(node, splitAxis, lowestCost, bestBinPos);

	//float parentArea = node->bounds.Area();
	//float parentCost = parentArea * node->count;
	//printf("parentCost: %f\n", parentCost);
	//printf("parentArea: %f\n", parentArea);
	if (lowestCost >= node->bounds.Area() * node->count)
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

	AABB intersectRL = left_right.first.Intersection(left_right.second);
	if (intersectRL.Area() / node->bounds.Area() > spatialSplitConstraint)
	{
		//printf("union Area %f\n", unionRL.Area());
		//TODO: Implement spatial splitting

		int createdReferenceCount = 0;
		printf("start\n");

		int end = node->first + node->count;

		for (int i = node->first; i < end; i++)
		{
			Triangle tri = (*primitives)[indices[i]];

			aabb = tri.GetAABB();

			if (aabb.bmin3.x < splitPos && aabb.bmax3.x > splitPos)
			{
				if (tri.GetCentroid().x <= splitPos) 
				{
					indices.insert(indices.begin()+ node->first + 1, indices[i]);
					j++;
				}
				else 
				{
					indices.insert(indices.begin() + node->first + j - node->first + 2, indices[i]);
				}
				node->count += 1;

				//printf("increment\n");
				//indices[node->count + createdReferenceCount] = indices[i];

				//createdReferenceCount++;

			}
		}

	}

	BVHNode* left = node->left;
	left->count = j - node->first + 1;
	left->first = node->first;
	left->bounds = left_right.first;// CalculateBounds(left->first, left->count);

	BVHNode* right = node->right;
	right->count = node->count - left->count;
	right->first = left->first + left->count;
	right->bounds = left_right.second;// CalculateBounds(right->first, right->count);

	if (left->count == node->count || right->count == node->count) {
		node->left = nullptr;
		node->right = nullptr;
		node->isLeaf = true;
		return false;
	}
return true;
}

pair<AABB, AABB> BVH::SplitAABB(BVHNode* node, int splitAxis, float& lowestCost, float& bestBinPos)
{
	AABB bestLeft, bestRight;

	float axisWidths[3] = { node->bounds.bmax3.x - node->bounds.bmin3.x, node->bounds.bmax3.y - node->bounds.bmin3.y, node->bounds.bmax3.z - node->bounds.bmin3.z };
	float binPositions[3] = { node->bounds.bmin3.x, node->bounds.bmin3.y, node->bounds.bmin3.z };
	float binDist = axisWidths[splitAxis] * iBinCount;

	for (int j = 0; j < binCount - 1; j++)
	{
		binPositions[splitAxis] += binDist;
		//AABB left, right;
		float3 leftMinBound = minb;
		float3 leftMaxBound = maxb;
		float3 rightMinBound = minb;
		float3 rightMaxBound = maxb;
		int leftCount = 0;
		int rightCount = 0;
		for (int i = node->first; i < node->first + node->count; i++)
		{
			float p = 0;
			switch (splitAxis)
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
			if (p < binPositions[splitAxis])
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
				rightMinBound.x = min(rightMinBound.x, aabb.bmin3.x);
				rightMinBound.y = min(rightMinBound.y, aabb.bmin3.y);
				rightMinBound.z = min(rightMinBound.z, aabb.bmin3.z);
				rightMaxBound.x = max(rightMaxBound.x, aabb.bmax3.x);
				rightMaxBound.y = max(rightMaxBound.y, aabb.bmax3.y);
				rightMaxBound.z = max(rightMaxBound.z, aabb.bmax3.z);
				rightCount++;
			}
		}

		AABB left = AABB(leftMinBound, leftMaxBound);
		AABB right = AABB(rightMinBound, rightMaxBound);
		//SAH
		float leftArea = left.Area();
		float rightArea = right.Area();
		leftArea = isinf(leftArea) ? 0 : leftArea;
		rightArea = isinf(rightArea) ? 0 : rightArea;
		float cost = leftArea * leftCount + rightArea * rightCount;

		if (cost < lowestCost)
		{
			lowestCost = cost;
			bestBinPos = binPositions[splitAxis];
			bestLeft = left;
			bestRight = right;
		}
	}

	return make_pair(bestLeft, bestRight);
}

pair<AABB, AABB> BVH::SpatialSplitAABB(BVHNode* node, int splitAxis, float& lowestSpatialCost, float binPos)
{
	AABB bestLeft, bestRight;

	float3 leftMinBound = minb;
	float3 leftMaxBound = maxb;
	float3 rightMinBound = minb;
	float3 rightMaxBound = maxb;
	int leftCount = 0;
	int rightCount = 0;

	for (int i = node->first; i < node->first + node->count; i++)
	{
		Triangle tri = (*primitives)[indices[i]];
		//AABB aabb = (*primitives)[indices[i]].GetAABB();
		float3 leftMax = float3(binPos, node->bounds.bmax3.y, node->bounds.bmax3.y);
		float3 rightMin = float3(binPos, node->bounds.bmin3.y, node->bounds.bmin3.y);
		AABB leftClipBox(node->bounds.bmin3, leftMax);
		AABB rightClipBox(rightMin, node->bounds.bmax3);		


		float3* vertices = tri.GetVertices();

		if (vertices[0].x <= binPos && vertices[1].x <= binPos && vertices[2].x <= binPos) //All points left
		{
			AABB aabb = (*primitives)[indices[i]].GetAABB();
			leftMinBound.x = min(leftMinBound.x, aabb.bmin3.x);
			leftMinBound.y = min(leftMinBound.y, aabb.bmin3.y);
			leftMinBound.z = min(leftMinBound.z, aabb.bmin3.z);
			leftMaxBound.x = max(leftMaxBound.x, aabb.bmax3.x);
			leftMaxBound.y = max(leftMaxBound.y, aabb.bmax3.y);
			leftMaxBound.z = max(leftMaxBound.z, aabb.bmax3.z);
			leftCount++;
		}
		else if (vertices[0].x > binPos && vertices[1].x > binPos && vertices[2].x > binPos) //All points right
		{
			AABB aabb = (*primitives)[indices[i]].GetAABB();
			rightMinBound.x = min(rightMinBound.x, aabb.bmin3.x);
			rightMinBound.y = min(rightMinBound.y, aabb.bmin3.y);
			rightMinBound.z = min(rightMinBound.z, aabb.bmin3.z);
			rightMaxBound.x = max(rightMaxBound.x, aabb.bmax3.x);
			rightMaxBound.y = max(rightMaxBound.y, aabb.bmax3.y);
			rightMaxBound.z = max(rightMaxBound.z, aabb.bmax3.z);
			rightCount++;
		}
		else //Some points right, some left
		{
			//LeftBox
			vector<float3> leftPolyVerts = ClipTriangle(tri, leftClipBox);
			for (float3 p : leftPolyVerts)
			{
				leftMinBound.x = min(leftMinBound.x, p.x);
				leftMinBound.y = min(leftMinBound.y, p.y);
				leftMinBound.z = min(leftMinBound.z, p.z);
				leftMaxBound.x = max(leftMaxBound.x, p.x);
				leftMaxBound.y = max(leftMaxBound.y, p.y);
				leftMaxBound.z = max(leftMaxBound.z, p.z);
				leftCount++;
			}

			//RightBox
			vector<float3> rightPolyVerts = ClipTriangle(tri, rightClipBox);
			for (float3 p : rightPolyVerts)
			{
				rightMinBound.x = min(rightMinBound.x, p.x);
				rightMinBound.y = min(rightMinBound.y, p.y);
				rightMinBound.z = min(rightMinBound.z, p.z);
				rightMaxBound.x = max(rightMaxBound.x, p.x);
				rightMaxBound.y = max(rightMaxBound.y, p.y);
				rightMaxBound.z = max(rightMaxBound.z, p.z);
				rightCount++;
			}
		}

	}
}

//Sutherland-Hodgman Algorithm: https://en.wikipedia.org/wiki/Sutherland%E2%80%93Hodgman_algorithm
vector<float3> BVH::ClipTriangle(Triangle& tri, AABB& clipBox)
{
	vector<float3> out;
	//TODO: implement all cases;
	float3* vertices = tri.GetVertices();

	ClipPlane* clipPlanes = GetClipPlanes(clipBox);

	for (int i = 0; i < 6; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			float p1Dist = clipPlanes[i].Distance(vertices[j]);
			float p2Dist = clipPlanes[i].Distance(vertices[j + 1]);

			bool p1InFront = p1Dist >= 0;
			bool p1Behind = !p1InFront;
			bool p2InFront = p2Dist >= 0;
			bool p2Behind = !p2InFront;

			if (p1InFront && p2InFront) 
			{
				out.push_back(vertices[i + 1]);
			}
			else if (p1InFront && p2Behind) 
			{
				float alpha = p1Dist / (p1Dist + p2Dist);
				float3 intersection = lerp(vertices[j], vertices[j + 1], alpha);
				out.push_back(intersection);
			}
			else if (p1Behind && p2InFront)
			{
				float alpha = p1Dist / (p1Dist + p2Dist);
				float3 intersection = lerp(vertices[j], vertices[j + 1], alpha);
				out.push_back(intersection);
				out.push_back(vertices[i + 1]);
			}
		}
	}
	return out;
}

ClipPlane* BVH::GetClipPlanes(AABB& clipBox)
{
	ClipPlane out[6] = {};

	out[0].p = clipBox.bmin3;
	out[0].n = float3(1, 0, 0);
	out[1].p = clipBox.bmin3;
	out[1].n = float3(0, 1, 0);
	out[2].p = clipBox.bmin3;
	out[2].n = float3(0, 0, 1);

	out[3].p = clipBox.bmax3;
	out[3].n = float3(-1, 0, 0);
	out[4].p = clipBox.bmax3;
	out[4].n = float3(0, -1, 0);
	out[5].p = clipBox.bmax3;
	out[5].n = float3(0, 0, -1);

	return out;
}

int BVH::countNodes(const BVHNode& node) const
{
	if (node.isLeaf) return 1;

	return 1 + countNodes(*node.left) + countNodes(*node.right);
}