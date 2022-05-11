/*****************************************************************************
* @File : D:\MyGit\RayTracingManual\RealTimeRayTracing\BVH.h
* @Author : Wang ZhiHao
* @Date : 2022/4/13 16:29
*****************************************************************************/

#pragma once

#include <iostream>
#include <vector>

#include "CSObject.h"

struct BVH_Node
{
	BoundingBox AABB;
	int TriangleIndex = -1;
	int NodeIndex = -1;
	std::shared_ptr<BVH_Node> pParentNode = nullptr;
	std::shared_ptr<BVH_Node> pRightBrotherNode = nullptr;
	std::shared_ptr<BVH_Node> pLeftChildNode = nullptr;
	std::shared_ptr<BVH_Node> pRightChildNode = nullptr;
};

class BVH
{
public:
	enum
	{
		TYPE_X,
		TYPE_Y,
		TYPE_Z,
	};
protected:
	std::shared_ptr<BVH_Node> m_rootNode;
	std::vector<CSBVH_Node> m_vecCSBVHNode;
	static int m_nodeIndex;
public:
	BVH(std::vector<Triangle> vecObj);

public:
	int GetCSBVHNodeWidth();
	int GetCSBVHNodeStride();

	std::vector<CSBVH_Node>& GetCSBVHNode() { return m_vecCSBVHNode; }
protected:
	std::shared_ptr<BVH_Node> RecursiveBuild(std::vector<Triangle> vecObj,std::shared_ptr<BVH_Node> parentNode);
	void PreOrderRecurive(std::shared_ptr<BVH_Node> node);
	void MakeLinkedlistToVec();
};

