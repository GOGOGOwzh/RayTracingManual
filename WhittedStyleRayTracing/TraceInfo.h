/*****************************************************************************
* @File : D:\MyGit\RayTracingManual\RayTracingManual\WhittedStyleRayTracing\TraceInfo.h
* @Author : Wang ZhiHao
* @Date : 2022/1/13 15:16
*****************************************************************************/

#pragma once
#include "Vec.h"
#include "Ray.h"
#include <vector>

class Object;

class Light;
class Material;

struct TraceInfo
{
	bool FrontFace = true;
	unsigned int TriangleIndex = 0;
	float Fraction = std::numeric_limits<float>::max();
	Vector2f UV; 
	Vector2f ST; //这个是重心坐标插值，用来插值UV的
	Vector3f HitNormal;
	Point3 HitPoint;

	Object* HitObj = nullptr;
	std::shared_ptr<Material> HitObjMat = nullptr;
	std::shared_ptr<Ray> HitRay = nullptr;
	std::vector<std::shared_ptr<Light>> Lights;

	bool HaveTrace() {
		return HitObj && HitObjMat;
	}
	void CheckFrontFace()
	{
		FrontFace = HitNormal.DotProduct(HitRay->GetDir().Nor()) < 0;
		//hitNormal = frontFace ? hitNormal : -hitNormal;
	}
};