/*****************************************************************************
* @File : D:\MyGit\RayTracingManual\RayTracingManual\WhittedStyleRayTracing\MeshTriangle.h
* @Author : Wang ZhiHao
* @Date : 2022/1/20 15:00
*****************************************************************************/

#pragma once
#include "Object.h"
#include <vector>

class BVH;
class Triangle;

class MeshTriangle :public Object
{
public:
	MeshTriangle():Object(TYPE_MESH_TRIANGLE){}
	MeshTriangle(const Vector3f* verts, const unsigned int* vertsIndex, const Vector2f* uvIndex, const unsigned int triangleCount);
	MeshTriangle(const std::string& fileName,const Point3 & pos,const float scale = 1.0f);

public:
	virtual bool RayTrace(const Point3& pos, const Vector3f& dir, std::optional<TraceInfo>& traceInfo) override;
	virtual bool RayTraceWithBVH(const Point3& pos, const Vector3f& dir, std::optional<TraceInfo>& traceInfo) override;
	virtual bool GetSurfaceProperty(const std::optional<TraceInfo>& traceInfo, SurfaceProperty& proerty) override;

	Color3 GetDiffuseColor(const Vector2f& uv,const int matIndex = -1);

protected:
	void ConstructTriangles();
	void ConstructTriangles(const std::string& fileName);

protected:
	unsigned m_iTotalVertsCount = 0;
	unsigned m_iVertsCount = 0;
	unsigned m_iTriangleCount = 0;
	unsigned* m_pVertsIndex = nullptr;
	Vector3f* m_pVerts = nullptr;
	Vector2f* m_pVertsUVIndex = nullptr;
	std::string m_strObjName;
	std::vector<std::shared_ptr<Triangle>> m_vecTriangles;

	std::shared_ptr<BVH> m_pBVH = nullptr;
};

