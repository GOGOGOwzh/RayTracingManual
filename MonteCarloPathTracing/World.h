/*****************************************************************************
* @File : D:\Games101\RayTracingManual\RayTracingManual\WhittedStyleRayTracing\World.h
* @Author : Wang ZhiHao
* @Date : 2022/1/13 11:51
*****************************************************************************/

#pragma once
#include "Vec.h"
#include "Ray.h"
#include <optional>
#include <vector>

class Object;
class Light;
class BVH;
struct TraceInfo;

class World
{
	using VecWorldObj = std::vector<std::shared_ptr<Object>>;
	using VecWorldLight = std::vector<std::shared_ptr<Light>>;

public:
	World();
	~World();

public:
	void CreateOnlyDiffuseWorldObj();
	void CreateCookTorranceRelectWorldObj();
	void CreateCookTorranceRelectRefractWorldObj();

	std::optional<TraceInfo> RayTrace(const Ray& ray) const;
	Color3 RayTraceColor(const Ray& ray,int depth) const;
	void SampleAreaLight(Vector3f& pos, Vector3f & normal, float& pdf) const ;
	void SampleLight(Vector3f& pos, Vector3f& normal, float& pdf) const;

	void SetUseBVH(bool bUse) {
		m_bUseBVH = bUse;
	}
protected:
	std::optional<TraceInfo> RayTraceWithBVH(const Ray& ray) const;
	std::optional<TraceInfo> RayTrace(const Point3& pos, const Vector3f& dir) const;
	void BuildBVH();

protected:
	int m_iSceneMaxDepth = 5;
	VecWorldObj m_vecWorldObject;
	VecWorldLight m_vecLights;
	Color3 m_backgroundColor = Color3(1);
	std::shared_ptr<BVH> m_pBVH;

	bool m_bUseBVH = true;
};

