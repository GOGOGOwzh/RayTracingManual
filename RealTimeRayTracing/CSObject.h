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

struct BoundingBox
{
	Vec3 MinVert;
	Vec3 MaxVert;
	Vec3 CenterVert;
};

struct Triangle
{
	int Index;
	float Area;
	Vec3 Point[3];
	Vec3 PointNoraml[3];
	Vec2 UVIndex[3];
	Vec3 TriangleNormal;
	BoundingBox AABB;
	Material Mat;
};

struct CSBVH_Node
{
	BoundingBox AABB;
	int TriangleIndex;
};

inline BoundingBox Union(const BoundingBox& box1, const BoundingBox& box2)
{
	auto min_vert = Min(box1.MinVert, box2.MinVert);
	auto max_vert = Max(box1.MaxVert, box2.MaxVert);

	BoundingBox unionBox;
	unionBox.MinVert = min_vert;
	unionBox.MaxVert = max_vert;
	return unionBox;
}

inline BoundingBox Union(const BoundingBox& box1, const Vec3& vec)
{
	auto min_vert = Min(box1.MinVert, vec);
	auto max_vert = Max(box1.MaxVert, vec);

	BoundingBox unionBox;
	unionBox.MinVert = min_vert;
	unionBox.MaxVert = max_vert;

	return unionBox;
}

inline void CalcTriangleNormalAndArea(Triangle& triangle)
{
	auto p0 = triangle.Point[0];	auto p1 = triangle.Point[1];	auto p2 = triangle.Point[2];
	auto E1 = p1 - p0; auto E2 = p2 - p0;

	auto E1XE2 = E1.Cross(E2);
	E1XE2.Normalize();
	triangle.TriangleNormal = E1XE2;
	triangle.Area = E1XE2.Length() * 0.5f;
}

inline void BuildBoundingBox(Triangle& triangle)
{
	auto MinVert = Min(triangle.Point[0], Min(triangle.Point[1], triangle.Point[2]));
	auto MaxVert = Max(triangle.Point[0], Max(triangle.Point[1], triangle.Point[2]));

	auto CenterVert = (MaxVert + MinVert) * 0.5f;

	BoundingBox box;
	box.MaxVert = MaxVert;
	box.MinVert = MinVert;
	box.CenterVert = CenterVert;
	triangle.AABB = box;
}