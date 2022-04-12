/*****************************************************************************
* @File : D:\MyGit\RayTracingManual\RealTimeRayTracing\CSObject.h
* @Author : Wang ZhiHao
* @Date : 2022/4/12 10:18
*****************************************************************************/

#pragma once
#include "Common.h"

enum 
{
	MAT_LAMBERT = 1,
	MAT_MC_DIFFUSE,
};

struct CB
{
	Mat4x4 VPInvert;
	Vec3 CameraPos;

	UINT SampleCount;
};

struct Material
{
	UINT Type;
	Vec3 Albedo;
	Vec3 Emission;
};

struct Sphere
{
	Vec3 Center;
	float Radius = 1.0f;
	Material Mat;
};