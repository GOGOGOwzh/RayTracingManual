/*****************************************************************************
* @File : D:\MyGit\RayTracingManual\RealTimeRayTracing\World.h
* @Author : Wang ZhiHao
* @Date : 2022/3/24 16:49
*****************************************************************************/

#pragma once
#include "Common.h"
#include "CSObject.h"

class MeshTriangle;
class BVH;
class World
{
public:
	void Init();
public:
	void Tick(DWORD deltaTime);
	void Render();

public:
	int GetSphereByteWidth();
	int GetSphereStride();
	std::vector<Sphere>& GetSphereVec();

	int GetTriangleByteWidth();
	int GetTriangleStride();
	std::vector<Triangle>& GetTriangleVec();

	int GetBVHByteWidth();
	int GetBVHStride();
	std::vector<CSBVH_Node>& GetBVHVec();

protected:
	void CreateWorld();
	void CreateSphere();
	void CreateMeshTriangle();
	void AddTriangleToVec(MeshTriangle* pMesh);
	void BuildBVH();

protected:
	std::vector<Sphere> m_VecSphere;
	std::vector<Triangle> m_VecTriangles;
	std::shared_ptr<BVH> m_BVH= nullptr;
};

