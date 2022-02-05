#include "precomp.h"

int missingRefCount = 0;

BVH::BVH(vector<Triangle>* intersectables, uInt count, mat4 translation, bool diagnostics, float spatialSplitConstraint, float spatialSplitCost)
	:primitives(intersectables), n(count), translation(translation), diagnostics(diagnostics), spatialSplitConstraint(spatialSplitConstraint), spatialSplitCost(spatialSplitCost)
{
	invTranslation = translation.Inverted();
}

void BVH::ConstructBVH()
{
	//Allocate extra memory for indices when constructing the sbvh
	if (spatialSplitConstraint == 1)
		indices.resize(n);
	else
		indices.resize(n);


	for (int i = 0; i < n; i++) indices[i] = i;
	pool = new BVHNode[n * 3];
	root = &pool[0];
	poolPtr = 2;

	//subdivide node
	root->first = 0;
	root->count = n;
	root->bounds = CalculateBounds(root->first, root->count);
	root->parent = nullptr;

	SubdivideBVHNode(root);

	if (diagnostics)
	{
		printf("Number of nodes: %i\n", countNodes(*root));
		printf("\nNumber of references %i\n", root->count);
		printf("indices size: %i\n", indices.size());
		PrintRightLeaf(root);
		printf("Number of spatial splits %i\n", spatialSplitCount);
		printf("Number of missing refs %i\n", missingRefCount);
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

void BVH::PrintRightLeaf(BVHNode* node) {
	if (node->isLeaf) {
		printf("right:: First %i | count %i | sum %i\n", node->first, node->count, node->first + node->count);
	}
	else {
		PrintRightLeaf(node->right);
	}
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
		//return dummy;
		Intersection intersection = GetClosestIntersectionInNode(r, node, nChecks);
		return intersection;
	}
	else
	{
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
		int ind = indices[i];
		Triangle tri = (*primitives)[ind];
		intersection = tri.Intersect(r);
		nChecks++;

		if (intersection.intersect && (closest_intersection.intersect == false || intersection.t < closest_intersection.t))
		{
			closest_intersection = intersection;
		}
	}


	closest_intersection.nAABBandTriChecks = nChecks;

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
	}

	return AABB(minBound, maxBound);
}

void BVH::SubdivideBVHNode(BVHNode* node)
{
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
	AABB aabb;
	float lowestCost = minb.x;
	float bestBinPos;

	//Binning
	int splitAxis = node->bounds.LongestAxis();

	pair<AABB, AABB> left_right = SplitAABB(node, splitAxis, lowestCost, bestBinPos);

	if (lowestCost >= node->bounds.Area() * node->count)
	{
		node->left = nullptr;
		node->right = nullptr;
		node->isLeaf = true;
		return false;
	}

	int j = node->first - 1;

	for (int i = node->first; i < node->first + node->count; i++)
	{
		AABB primitiveAABB = (*primitives)[indices[i]].GetAABB();
		float AABBCentroid = primitiveAABB.Center(splitAxis);
		
		if (AABBCentroid < bestBinPos)
			swap(indices[++j], indices[i]);
	}

	AABB intersectRL = left_right.first.Intersection(left_right.second);
	if (intersectRL.Area() / node->bounds.Area() > spatialSplitConstraint)
	{
		float spatialCost = 0;
		pair<AABB, AABB> spatial_left_right = SpatialSplitAABB(node, splitAxis, spatialCost, bestBinPos);

		if (spatialCost < lowestCost)
		{
			spatialSplitCount++;
			left_right.first = spatial_left_right.first;
			left_right.second = spatial_left_right.second;

			int createdReferenceCount = 0;

			int end = node->first + node->count;
			int referenceDupCount = 0;

			vector<uInt> inds(indices.begin() + node->first, indices.begin() + end);

			for (int i = 0; i < node->count; i++)
			{
				Triangle tri = (*primitives)[inds[i]];

				aabb = tri.GetAABB();
				float3* verts = tri.GetVertices();

				switch (splitAxis)
				{
				case(0):
					if (!(verts[0].x <= bestBinPos + 0.01f && verts[1].x <= bestBinPos + 0.01f && verts[2].x <= bestBinPos + 0.01f)
						&& !(verts[0].x >= bestBinPos - 0.01f && verts[1].x >= bestBinPos - 0.01f && verts[2].x >= bestBinPos - 0.01f))//  aabb.bmin3.x < splitPos && aabb.bmax3.x > splitPos)
					{
						if (tri.GetCentroid().x <= bestBinPos)
						{
							indices.insert(indices.begin() + j + 1, inds[i]);
						}
						else
						{
							indices.insert(indices.begin() + node->first + 1, inds[i]);
							j++;
						}
						referenceDupCount += 1;
						missingRefCount += 1;
					}
					break;
				case(1):
					if (!(verts[0].y <= bestBinPos + 0.01f && verts[1].y <= bestBinPos + 0.01f && verts[2].y <= bestBinPos + 0.01f)
						&& !(verts[0].y >= bestBinPos - 0.01f && verts[1].y >= bestBinPos - 0.01f && verts[2].y >= bestBinPos - 0.01f))//(aabb.bmin3.y < splitPos && aabb.bmax3.y > splitPos)
					{
						if (tri.GetCentroid().y <= bestBinPos)
						{
							indices.insert(indices.begin() + j + 1, inds[i]);
						}
						else
						{
							indices.insert(indices.begin() + node->first + 1, inds[i]);
							j++;
						}
						referenceDupCount += 1;
						missingRefCount += 1;
					}
					break;
				case(2):
					if (!(verts[0].z <= bestBinPos + 0.01f && verts[1].z <= bestBinPos + 0.01f && verts[2].z <= bestBinPos + 0.01f)
						&& !(verts[0].z >= bestBinPos - 0.01f && verts[1].z >= bestBinPos - 0.01f && verts[2].z >= bestBinPos - 0.01f))//(aabb.bmin3.z < splitPos && aabb.bmax3.z > splitPos)
					{
						if (tri.GetCentroid().z <= bestBinPos)
						{
							indices.insert(indices.begin() + j + 1, inds[i]);
						}
						else
						{
							indices.insert(indices.begin() + node->first + 1, inds[i]);
							j++;
						}
						referenceDupCount += 1;
						missingRefCount += 1;
					}
					break;
				}
			}
			/*printf("root count before: %i\n", root->count);
			printf("count before: %i\n", node->count);*/
			UpdateBVHNodeCounts(node, referenceDupCount);
			UpdateBVHNodeFirsts(node, referenceDupCount);

			/*printf("ExitNodeCount: %i\n", node->count);
			printf("root count after: %i\n", root->count);
			printf("count after: %i\n", node->count);
			printf("indices count after: %i\n", indices.size());*/
		}
	}
	AABB intersect = left_right.first.Intersection(left_right.second);


	if (node->left == nullptr || node->right == nullptr) {
		int dummy = 1;
	}
	BVHNode* left = node->left;
	left->count = j - node->first + 1;
	left->first = node->first;
	left->bounds = left_right.first;// CalculateBounds(left->first, left->count);
	left->parent = node;

	BVHNode* right = node->right;
	right->count = node->count - left->count;
	right->first = left->first + left->count;
	right->bounds = left_right.second;// CalculateBounds(right->first, right->count);
	right->parent = node;

	if (left->count == node->count || right->count == node->count) {
		node->left = nullptr;
		node->right = nullptr;
		node->isLeaf = true;
		return false;
	}

	return true;
}

void BVH::UpdateBVHNodeFirsts(BVHNode* node, int amount)
{
	if (node->parent == nullptr) return;

	BVHNode* rightNode = node->parent->right;

	if (rightNode != node)
	{
		if (rightNode->first == 0) {
			printf("is 0");
		}
			rightNode->first += amount;
	}

	UpdateBVHNodeFirsts(node->parent, amount);
}

void BVH::UpdateBVHNodeCounts(BVHNode* node, int amount)
{
	if (amount == 0) return;

	node->count += amount;

	if (node->parent != nullptr) {

		UpdateBVHNodeCounts(node->parent, amount);
	}
}

pair<AABB, AABB> BVH::SplitAABB(BVHNode* node, int splitAxis, float& lowestCost, float& bestBinPos)
{
	AABB bestLeft, bestRight;

	float axisWidths[3] = { node->bounds.bmax3.x - node->bounds.bmin3.x, node->bounds.bmax3.y - node->bounds.bmin3.y, node->bounds.bmax3.z - node->bounds.bmin3.z };
	float binPositions[3] = { node->bounds.bmin3.x, node->bounds.bmin3.y, node->bounds.bmin3.z };
	float binDist = axisWidths[splitAxis] * iBinCount;
	int lcount = 0;
	int rcount = 0;

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
			//float p = 0;
			//switch (splitAxis)
			//{
			//case(0): //x-axis
			//	p = (*primitives)[indices[i]].GetCentroid().x;
			//	break;
			//case(1): //y-axis
			//	p = (*primitives)[indices[i]].GetCentroid().y;
			//	break;
			//case(2): //z-axis
			//	p = (*primitives)[indices[i]].GetCentroid().z;
			//	break;
			//}

			float p = (*primitives)[indices[i]].GetAABB().Center(splitAxis);

			AABB aabb = (*primitives)[indices[i]].GetAABB();

			Triangle tri = (*primitives)[indices[i]];
			float3* vertices = tri.GetVertices();

			//CheckAll if all vertices are inside of the aabb from this node
			if (   vertices[0].x <= node->bounds.bmin3.x - 0.01f || vertices[0].y <= node->bounds.bmin3.y - 0.01f || vertices[0].z <= node->bounds.bmin3.z - 0.01f
				|| vertices[0].x >= node->bounds.bmax3.x + 0.01f || vertices[0].y >= node->bounds.bmax3.y + 0.01f || vertices[0].z >= node->bounds.bmax3.z + 0.01f
				|| vertices[1].x <= node->bounds.bmin3.x - 0.01f || vertices[1].y <= node->bounds.bmin3.y - 0.01f || vertices[1].z <= node->bounds.bmin3.z - 0.01f
				|| vertices[1].x >= node->bounds.bmax3.x + 0.01f || vertices[1].y >= node->bounds.bmax3.y + 0.01f || vertices[1].z >= node->bounds.bmax3.z + 0.01f
				|| vertices[2].x <= node->bounds.bmin3.x - 0.01f || vertices[2].y <= node->bounds.bmin3.y - 0.01f || vertices[2].z <= node->bounds.bmin3.z - 0.01f
				|| vertices[2].x >= node->bounds.bmax3.x + 0.01f || vertices[2].y >= node->bounds.bmax3.y + 0.01f || vertices[2].z >= node->bounds.bmax3.z + 0.01f)
			{
				////Adjust tri aabb to the cliped tri
				vector<float3> verts = ClipTriangle(tri, node->bounds);

				float3 minbound = minb;
				float3 maxbound = maxb;
				for (float3 vert : verts)
				{
					minbound.x = min(minbound.x, vert.x);
					minbound.y = min(minbound.y, vert.y);
					minbound.z = min(minbound.z, vert.z);
					maxbound.x = max(maxbound.x, vert.x);
					maxbound.y = max(maxbound.y, vert.y);
					maxbound.z = max(maxbound.z, vert.z);
				}
				aabb.bmin3 = minbound;
				aabb.bmax3 = maxbound;
				p = aabb.Center(splitAxis);
			}

			if (p < binPositions[splitAxis])
			{
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
			lcount = leftCount;
			rcount = rightCount;
		}
	}
	//printf("bestBinPos %f\n", bestBinPos);
	//printf("normal split: leftArea %f, rightArea %f, leftCount %i, rightCount %i\n", bestLeft.Area(), bestRight.Area(), lcount, rcount);

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
		float3 leftMax;
		float3 rightMin;

		if (splitAxis == 0)
		{
			leftMax = float3(binPos, node->bounds.bmax3.y, node->bounds.bmax3.z);
			rightMin = float3(binPos, node->bounds.bmin3.y, node->bounds.bmin3.z);
		}
		else if (splitAxis == 1)
		{
			leftMax = float3(node->bounds.bmax3.x, binPos, node->bounds.bmax3.z);
			rightMin = float3(node->bounds.bmin3.x, binPos, node->bounds.bmin3.z);
		}
		else
		{
			leftMax = float3(node->bounds.bmax3.x, node->bounds.bmax3.y, binPos);
			rightMin = float3(node->bounds.bmin3.x, node->bounds.bmin3.y, binPos);
		}

		AABB leftClipBox(node->bounds.bmin3, leftMax);
		AABB rightClipBox(rightMin, node->bounds.bmax3);

		float3* vertices = tri.GetVertices();

		float3 points;

		switch (splitAxis)
		{
		case(0):
			points = float3(vertices[0].x, vertices[1].x, vertices[2].x);
			break;
		case(1):
			points = float3(vertices[0].y, vertices[1].y, vertices[2].y);
			break;
		case(2):
			points = float3(vertices[0].z, vertices[1].z, vertices[2].z);
			break;
		}

		AABB aabb = (*primitives)[indices[i]].GetAABB();

		if (points.x <= binPos + .01f && points.y <= binPos + .01f && points.z <= binPos + .01f) //All points left
		{
			leftMinBound.x = min(leftMinBound.x, aabb.bmin3.x);
			leftMinBound.y = min(leftMinBound.y, aabb.bmin3.y);
			leftMinBound.z = min(leftMinBound.z, aabb.bmin3.z);
			leftMaxBound.x = max(leftMaxBound.x, aabb.bmax3.x);
			leftMaxBound.y = max(leftMaxBound.y, aabb.bmax3.y);
			leftMaxBound.z = max(leftMaxBound.z, aabb.bmax3.z);
			leftCount++;
		}
		else if (points.x >= binPos - .01f && points.y >= binPos - .01f && points.z >= binPos - .01f) //All points right
		{
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
			vector<float3> rightPolyVerts = ClipTriangle(tri, rightClipBox);

			if (leftPolyVerts.size() == 0)
			{
				leftMinBound.x = min(leftMinBound.x, aabb.bmin3.x);
				leftMinBound.y = min(leftMinBound.y, aabb.bmin3.y);
				leftMinBound.z = min(leftMinBound.z, aabb.bmin3.z);
				leftMaxBound.x = max(leftMaxBound.x, aabb.bmax3.x);
				leftMaxBound.y = max(leftMaxBound.y, aabb.bmax3.y);
				leftMaxBound.z = max(leftMaxBound.z, aabb.bmax3.z);
				leftCount++;
			}
			else if (rightPolyVerts.size() == 0)
			{
				rightMinBound.x = min(rightMinBound.x, aabb.bmin3.x);
				rightMinBound.y = min(rightMinBound.y, aabb.bmin3.y);
				rightMinBound.z = min(rightMinBound.z, aabb.bmin3.z);
				rightMaxBound.x = max(rightMaxBound.x, aabb.bmax3.x);
				rightMaxBound.y = max(rightMaxBound.y, aabb.bmax3.y);
				rightMaxBound.z = max(rightMaxBound.z, aabb.bmax3.z);
				rightCount++;
			}
			else {

				for (float3 p : leftPolyVerts)
				{
					leftMinBound.x = min(leftMinBound.x, p.x);
					leftMinBound.y = min(leftMinBound.y, p.y);
					leftMinBound.z = min(leftMinBound.z, p.z);
					leftMaxBound.x = max(leftMaxBound.x, p.x);
					leftMaxBound.y = max(leftMaxBound.y, p.y);
					leftMaxBound.z = max(leftMaxBound.z, p.z);
				}
				leftCount++;

				//RightBox
				for (float3 p : rightPolyVerts)
				{
					rightMinBound.x = min(rightMinBound.x, p.x);
					rightMinBound.y = min(rightMinBound.y, p.y);
					rightMinBound.z = min(rightMinBound.z, p.z);
					rightMaxBound.x = max(rightMaxBound.x, p.x);
					rightMaxBound.y = max(rightMaxBound.y, p.y);
					rightMaxBound.z = max(rightMaxBound.z, p.z);
				}
				rightCount++;
			}
		}
	}

	AABB left = AABB(leftMinBound, leftMaxBound);
	AABB right = AABB(rightMinBound, rightMaxBound);
	//SAH
	float leftArea = left.Area();
	float rightArea = right.Area();
	leftArea = isinf(leftArea) ? 0 : leftArea; //Look at this hier gaat het fout
	rightArea = isinf(rightArea) ? 0 : rightArea;

	float cost = spatialSplitCost + leftArea * leftCount + rightArea * rightCount;
	lowestSpatialCost = cost;

	return make_pair(left, right);
}

//Sutherland-Hodgman Algorithm: https://en.wikipedia.org/wiki/Sutherland%E2%80%93Hodgman_algorithm
vector<float3> BVH::ClipTriangle(Triangle& tri, AABB& clipBox)
{
	vector<float3> out;
	float3* vertices = tri.GetVertices();

	out.push_back(vertices[2]);
	out.push_back(vertices[1]);
	out.push_back(vertices[0]);

	ClipPlane* clipPlanes = GetClipPlanes(clipBox);
	for (int i = 0; i < 6; i++)
	{
		vector<float3> input = out;
		out.clear();

		if (input.size() == 0)
		{
			return input;
		}

		float3 startPoint = input.back();

		for (float3 endPoint : input)
		{
			float p1Dist = clipPlanes[i].Distance(startPoint);
			float p2Dist = clipPlanes[i].Distance(endPoint);

			bool p1InFront = p1Dist >= -.001f;
			bool p1Behind = !p1InFront;
			bool p2InFront = p2Dist >= -.001f;
			bool p2Behind = !p2InFront;

			if (p1InFront && p2InFront)
			{
				out.push_back(endPoint);
			}
			else if (p1InFront && p2Behind)
			{
				float alpha = abs(p1Dist) / (abs(p1Dist) + abs(p2Dist));
				float3 intersection = lerp(startPoint, endPoint, alpha);
				out.push_back(intersection);

			}
			else if (p1Behind && p2InFront)
			{
				float alpha = abs(p1Dist) / (abs(p1Dist) + abs(p2Dist));
				float3 intersection = lerp(startPoint, endPoint, alpha);
				out.push_back(intersection);
				out.push_back(endPoint);
			}

			startPoint = endPoint;
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