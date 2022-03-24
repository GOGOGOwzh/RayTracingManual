/*****************************************************************************
* @File : D:\Games101\RayTracingManual\RayTracingManual\WhittedStyleRayTracing\Object.h
* @Author : Wang ZhiHao
* @Date : 2021/9/14 15:40
*****************************************************************************/

#pragma once
#include <iostream>
#include <optional>
#include <vector>

#include "Vec.h"

enum OBJ_TYPE
{
	TYPE_RAY,
	TYPE_CAMERA,
	TYPE_MAT,
	TYPE_LIGHT,
	TYPE_SPHERE,
	TYPE_MESH_TRIANGLE,
	TYPE_TRIANGLE,
	TYPE_BOUNDING_BOX,
};

class Material;
class BoundingBox;
struct TraceInfo;
struct SurfaceProperty
{
	Vector2f uv;
	Vector3f normal;
};


class Object
{
	using MATVEC = std::vector<std::shared_ptr<Material>>;
protected:
	std::string m_strName;
	char		m_objType;
	bool		m_bCanTrace = true;
	
	MATVEC m_objMatVec;
	Point3 m_pos;
	float m_scale = 1.0f;

	std::shared_ptr<BoundingBox> m_pBoundingBox = nullptr;
public:
	Object(char type):m_objType(type){}
	virtual ~Object() = default;

public:
	char GetObjType() {
		return m_objType;
	}

	void SetObjectName(std::string name) {
		m_strName = name;
	}

	const std::string & GetObjectName() {
		return m_strName;
	}

	virtual std::shared_ptr<Material> GetObjectMaterial(const unsigned char matIndex = 0) {
		if (matIndex >= m_objMatVec.size() || matIndex < 0)
		{
			return nullptr;
		}
		return m_objMatVec[matIndex];
	}

	bool CanTrace() {
		return m_bCanTrace;
	}

	std::shared_ptr<BoundingBox> GetBoundingBox() {
		return m_pBoundingBox;
	}

	virtual bool RayTrace(const Point3& pos, const Vector3f& dir, std::optional<TraceInfo> & traceInfo)
	{
		return false;
	};

	virtual bool RayTraceWithBVH(const Point3& pos, const Vector3f& dir, std::optional<TraceInfo>& traceInfo)
	{
		return false;
	};

	void PushMaterial(std::shared_ptr<Material> mat)
	{
		m_objMatVec.push_back(mat);
	}

	virtual bool GetSurfaceProperty(const std::optional<TraceInfo>& traceInfo, SurfaceProperty & proerty)
	{
		return false;
	}
};
