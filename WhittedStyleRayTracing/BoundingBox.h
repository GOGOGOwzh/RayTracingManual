/*****************************************************************************
* @File : D:\MyGit\RayTracingManual\RayTracingManual\WhittedStyleRayTracing\BoundingBox.h
* @Author : Wang ZhiHao
* @Date : 2022/1/27 10:37
*****************************************************************************/

#pragma once

#include "Vec.h"
#include "Ray.h"

class Sphere;
class Triangle;

class BoundingBox : public Object
{
public:
	enum EXT_TYPE
	{
		TYPE_X,
		TYPE_Y,
		TYPE_Z,
	};
public:
	Vector3f m_MinVert;
	Vector3f m_MaxVert;
	Vector3f m_CenterVert;
public:
	BoundingBox():Object(TYPE_BOUNDING_BOX) {
		m_MinVert = Vector3f::MakeMax();
		m_MaxVert = Vector3f::MakeMin();
	};
	BoundingBox(const Vector3f& minVert, const Vector3f& maxVert);
	BoundingBox(const Sphere* pShere);
	BoundingBox(const Triangle* pTriangle);

public:
	bool OverLaps(const BoundingBox& box);
	bool Inside(const Vector3f& vert);
	Vector3f Diagonal() {return m_MaxVert - m_MinVert; }

	float DiagonalLen() {
		Vector3f diagonal = m_MaxVert - m_MinVert;
		return std::sqrtf(diagonal.x * diagonal.x + diagonal.y * diagonal.y + diagonal.z * diagonal.z);
	}

	bool Trace(const Point3& pos, const Vector3f& dir, float& tenter, float& texit);
	virtual bool RayTrace(const Point3& pos, const Vector3f& dir, std::optional<TraceInfo>& traceInfo) override;

	char GetExType();

	BoundingBox Clone();

public:
	BoundingBox Union(const BoundingBox& box);
	BoundingBox Union(const Vector3f& vec);
};

