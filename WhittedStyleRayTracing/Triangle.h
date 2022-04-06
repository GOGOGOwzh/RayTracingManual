/*****************************************************************************
* @File : D:\MyGit\RayTracingManual\RayTracingManual\WhittedStyleRayTracing\Triangle.h
* @Author : Wang ZhiHao
* @Date : 2022/1/29 10:17
*****************************************************************************/

#pragma once
#include "Object.h"
#include "Vec.h"

class MeshTriangle;
class Triangle : public Object
{
public:
	Triangle(MeshTriangle* pMesh) :Object(TYPE_TRIANGLE) {
		PMesh = pMesh;
	}

	int MatIndex = -1;
	Vector3f Point[3];
	Vector3f PointNoraml[3];
	Vector2f UVIndex[3];
	Vector3f TriangleNormal;
	MeshTriangle* PMesh = nullptr;

	bool GetUVAndNormal(const Vector2f& st, Vector3f& normal, Vector2f& uv);
	void CalcTriangleNormal();
	void BuildBoundingBox();
	bool Trace(const Point3& pos, const Vector3f& dir, float& fraction, Vector2f& st);
	virtual bool RayTrace(const Point3& pos, const Vector3f& dir, std::optional<TraceInfo>& traceInfo) override;
	virtual bool RayTraceWithBVH(const Point3& pos, const Vector3f& dir, std::optional<TraceInfo>& traceInfo) override;
	virtual bool GetSurfaceProperty(const std::optional<TraceInfo>& traceInfo, SurfaceProperty& proerty) override;
	virtual std::shared_ptr<Material> GetObjectMaterial(const unsigned char matIndex = 0) override;
};