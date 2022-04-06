/*****************************************************************************
* @File : D:\MyGit\RayTracingManual\RayTracingManual\WhittedStyleRayTracing\Sphere.h
* @Author : Wang ZhiHao
* @Date : 2022/1/20 15:00
*****************************************************************************/

#pragma once
#include "Object.h"

class BVH;
class Sphere :public Object
{
protected:
	Point3 m_center;
	float m_radius = 0;
public:
	Point3 GetCenter() const {
		return m_center;
	}

	float GetRadius()const {
		return m_radius;
	}
public:
	Sphere();
	Sphere(const Point3& center, const float radius);
	virtual ~Sphere() = default;

	virtual bool RayTrace(const Point3& pos, const Vector3f& dir, std::optional<TraceInfo>& traceInfo) override;
	virtual bool RayTraceWithBVH(const Point3& pos, const Vector3f& dir, std::optional<TraceInfo>& traceInfo) override;
	virtual bool GetSurfaceProperty(const std::optional<TraceInfo>& traceInfo, SurfaceProperty& proerty) override;
};

