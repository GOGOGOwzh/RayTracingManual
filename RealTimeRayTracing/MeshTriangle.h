/*****************************************************************************
* @File : D:\MyGit\RayTracingManual\RealTimeRayTracing\MeshTriangle.h
* @Author : Wang ZhiHao
* @Date : 2022/4/13 16:27
*****************************************************************************/
#pragma once

#include "Common.h"
#include "CSObject.h"

class MeshTriangle
{
public:
	MeshTriangle(const Vec3* verts, const unsigned int* vertsIndex, const Vec2* uvIndex, const unsigned int triangleCount, const Material& mat);
	MeshTriangle(const std::string& fileName, const Vec3& pos, const Material &mat, const float scale = 1.0f);

	std::vector<Triangle>& GetTriangleVec() { return m_vecTriangles; }

	int GetTrianglesCount() { return m_iTriangleCount; }

protected:
	void ConstructTriangles();
	void ConstructTriangles(const std::string& fileName);


protected:
	Vec3 m_pos;
	float m_scale = 1.0f;
	unsigned m_iTotalVertsCount = 0;
	unsigned m_iVertsCount = 0;
	unsigned m_iTriangleCount = 0;
	unsigned* m_pVertsIndex = nullptr;
	Vec3* m_pVerts = nullptr;
	Vec2* m_pVertsUVIndex = nullptr;
	std::string m_strObjName;
	std::vector<Triangle> m_vecTriangles;

	Material m_Mat;
};

