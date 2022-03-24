/*****************************************************************************
* @File : D:\Games101\RayTracingManual\RayTracingManual\WhittedStyleRayTracing\BVH.h
* @Author : Wang ZhiHao
* @Date : 2022/1/28 14:52
*****************************************************************************/

#pragma once

#include <iostream>
#include <vector>

#include "BoundingBox.h"
#include "Ray.h"
#include "TraceInfo.h"

class Object;

struct BVH_Node
{
	Object* pObj = nullptr;
	std::shared_ptr<BVH_Node> pLeftNode = nullptr;
	std::shared_ptr<BVH_Node> pRightNode = nullptr;
	BoundingBox boundingBox;
	float area = 0;
};

class BVH
{
protected:
	std::vector<Object*> m_vecObj;
	std::shared_ptr<BVH_Node> m_rootNode;

public:
	BVH(std::vector<Object*> vecObj);

public:
	std::shared_ptr<BVH_Node> RecursiveBuild(std::vector<Object*> vecObj);
	bool RayTrace(const Point3& pos, const Vector3f& dir, std::optional<TraceInfo>& traceInfo);
	bool PreOrderRecurive(const Point3& pos, const Vector3f& dir,std::shared_ptr<BVH_Node> node,std::optional<TraceInfo>& traceInfo);
	void Sample(Vector3f& pos, Vector3f& normal, float& pdf);
	void getSample(std::shared_ptr<BVH_Node> node, float p, Vector3f& pos, Vector3f& normal, float& pdf);
};

