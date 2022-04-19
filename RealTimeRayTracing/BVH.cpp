#include "BVH.h"
#include <algorithm>

int GetBoundingBoxExtType(const BoundingBox & box)
{
	auto diagonal = box.MaxVert - box.MinVert;
	if (diagonal.x > diagonal.y && diagonal.x > diagonal.z)
	{
		return BVH::TYPE_X;
	}
	else if (diagonal.y > diagonal.z)
	{
		return BVH::TYPE_Y;
	}
	else {
		return BVH::TYPE_Z;
	}
}

BVH::BVH(std::vector<Triangle> vecObj)
{
	time_t start, stop;
	time(&start);
	std::cout << "BuildBVH¡­¡­" << std::endl;

	m_rootNode = RecursiveBuild(vecObj);

	MakeLinkedlistToVec();

	time(&stop);
	double diff = difftime(stop, start);
	int hrs = (int)diff / 3600;
	int mins = ((int)diff / 60) - (hrs * 60);
	int secs = (int)diff - (hrs * 3600) - (mins * 60);

	printf(
		"\rBVH Generation complete: \nTime Taken: %i hrs, %i mins, %i secs\n\n",
		hrs, mins, secs);
}

int BVH::GetCSBVHNodeWidth()
{
	return m_vecCSBVHNode.size() * sizeof(CSBVH_Node);
}

int BVH::GetCSBVHNodeStride()
{
	return sizeof(CSBVH_Node);
}

std::shared_ptr<BVH_Node> BVH::RecursiveBuild(std::vector<Triangle> vecObj)
{
	if (vecObj.size() <= 0)
	{
		return nullptr;
	}

	std::shared_ptr<BVH_Node> node = std::make_shared<BVH_Node>();
	if (vecObj.size() == 1)
	{
		node->AABB = vecObj[0].AABB;
		node->TriangleIndex = vecObj[0].Index;
	}
	else if (vecObj.size() == 2) {
		node->pLeftNode = RecursiveBuild(std::vector{ vecObj[0] });
		node->pRightNode = RecursiveBuild(std::vector{ vecObj[1] });

		node->AABB = Union(node->pLeftNode->AABB, node->pRightNode->AABB);
		return node;
	}
	else {
		BoundingBox box;
		for (auto& obj : vecObj)
		{
			box = Union(box, obj.AABB.CenterVert);
		}

		auto exType = GetBoundingBoxExtType(box);
		switch (exType)
		{
		case TYPE_X:
		{
			std::sort(vecObj.begin(), vecObj.end(), [](auto obj1, auto obj2) {
				return obj1.AABB.CenterVert.x >	obj2.AABB.CenterVert.x;
			});

			break;
		}
		case TYPE_Y:
		{
			std::sort(vecObj.begin(), vecObj.end(), [](auto obj1, auto obj2) {
				return obj1.AABB.CenterVert.y > obj2.AABB.CenterVert.y;
			});
			break;
		}
		default:
			std::sort(vecObj.begin(), vecObj.end(), [](auto obj1, auto obj2) {
				return obj1.AABB.CenterVert.z > obj2.AABB.CenterVert.z;
			});

			break;
		}

		int midIndex = vecObj.size() / 2;
		auto vecLeft = std::vector<Triangle>(vecObj.begin(), vecObj.begin() + midIndex);
		auto vecRight = std::vector<Triangle>(vecObj.begin() + midIndex, vecObj.end());

		assert(vecObj.size() == (vecLeft.size() + vecRight.size()));

		node->pLeftNode = RecursiveBuild(vecLeft);
		node->pRightNode = RecursiveBuild(vecRight);

		node->AABB = Union(node->pLeftNode->AABB, node->pRightNode->AABB);
	}

	return node;
}

void BVH::PreOrderRecurive(std::shared_ptr<BVH_Node> node)
{
	if (!node)
	{
		return;
	}
	CSBVH_Node csNode;
	csNode.AABB = node->AABB;
	csNode.TriangleIndex = node->TriangleIndex;
	m_vecCSBVHNode.push_back(csNode);

	PreOrderRecurive(node->pLeftNode);
	PreOrderRecurive(node->pRightNode);
}

void BVH::MakeLinkedlistToVec()
{

}
