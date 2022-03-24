#include "BVH.h"
#include "Global.h"
#include "TraceInfo.h"
#include <assert.h>

BVH::BVH(std::vector<Object*> vecObj):m_vecObj(vecObj)
{
	time_t start, stop;
	time(&start);
	std::cout << "BuildBVH¡­¡­" << std::endl;

	m_rootNode = RecursiveBuild(m_vecObj);

	time(&stop);
	double diff = difftime(stop, start);
	int hrs = (int)diff / 3600;
	int mins = ((int)diff / 60) - (hrs * 60);
	int secs = (int)diff - (hrs * 3600) - (mins * 60);

	printf(
		"\rBVH Generation complete: \nTime Taken: %i hrs, %i mins, %i secs\n\n",
		hrs, mins, secs);
}

std::shared_ptr<BVH_Node> BVH::RecursiveBuild(std::vector<Object*> vecObj)
{
	if (vecObj.size() <= 0)
	{
		return nullptr;
	}

	std::shared_ptr<BVH_Node> node = std::make_shared<BVH_Node>();
	if (vecObj.size() == 1)
	{
		node->boundingBox = vecObj[0]->GetBoundingBox()->Clone();
		node->pObj = vecObj[0];
	}
	else if (vecObj.size() == 2) {
		node->pLeftNode = RecursiveBuild(std::vector{ vecObj[0] });
		node->pRightNode = RecursiveBuild(std::vector{ vecObj[1] });

		node->boundingBox = Union(node->pLeftNode->boundingBox, node->pRightNode->boundingBox);
		return node;
	}
	else {
		BoundingBox box;
		for (auto & obj : vecObj)
		{
			box = Union(box, obj->GetBoundingBox()->m_CenterVert);
		}

		auto exType = box.GetExType();
		switch (exType)
		{
		case BoundingBox::TYPE_X:
		{
			std::sort(vecObj.begin(), vecObj.end(), [](auto obj1, auto obj2) {
				return obj1->GetBoundingBox()->m_CenterVert.x >
					obj2->GetBoundingBox()->m_CenterVert.x;
			});

			break;
		}
		case BoundingBox::TYPE_Y:
		{
			std::sort(vecObj.begin(), vecObj.end(), [](auto obj1, auto obj2) {
				return obj1->GetBoundingBox()->m_CenterVert.y >
					obj2->GetBoundingBox()->m_CenterVert.y;
			});
			break;
		}
		default:
			std::sort(vecObj.begin(), vecObj.end(), [](auto obj1, auto obj2) {
				return obj1->GetBoundingBox()->m_CenterVert.z >
					obj2->GetBoundingBox()->m_CenterVert.z;
			});

			break;
		}

		int midIndex = vecObj.size() / 2;
		auto vecLeft = std::vector<Object*>(vecObj.begin(), vecObj.begin() + midIndex);
		auto vecRight = std::vector<Object*>(vecObj.begin() + midIndex, vecObj.end());
		
		assert(vecObj.size() == (vecLeft.size() + vecRight.size()));

		node->pLeftNode = RecursiveBuild(vecLeft);
		node->pRightNode = RecursiveBuild(vecRight);

		node->boundingBox = Union(node->pLeftNode->boundingBox,node->pRightNode->boundingBox);
	}

	return node;
}

bool BVH::RayTrace(const Point3& pos, const Vector3f& dir, std::optional<TraceInfo>& traceInfo)
{
	if (!m_rootNode)
	{
		return false;
	}

	return PreOrderRecurive(pos, dir,m_rootNode, traceInfo);
}

bool BVH::PreOrderRecurive(const Point3& pos, const Vector3f& dir, std::shared_ptr<BVH_Node> node, std::optional<TraceInfo>& traceInfo)
{
	if (!node)
	{
		return false;
	}

	bool bTrace = node->boundingBox.RayTrace(pos, dir, traceInfo);
	if (!bTrace)
	{
		return false;
	}

	if (node->pObj)
	{
		return node->pObj->RayTraceWithBVH(pos, dir, traceInfo);
	}

	std::optional<TraceInfo> tmpTraceLeft;
	std::optional<TraceInfo> tmpTraceRight;
	tmpTraceLeft.emplace();
	tmpTraceRight.emplace();


	bool bLeft = PreOrderRecurive(pos, dir, node->pLeftNode, tmpTraceLeft);
	bool bRight = PreOrderRecurive(pos, dir, node->pRightNode, tmpTraceRight);

	if (bLeft && bRight)
	{
		traceInfo = tmpTraceLeft->Fraction < tmpTraceRight->Fraction ? tmpTraceLeft : tmpTraceRight;
	}
	else if (bLeft)
	{
		traceInfo = tmpTraceLeft;
	}
	else if (bRight)
	{
		traceInfo = tmpTraceRight;
	}
	
	return bLeft || bRight;
}
