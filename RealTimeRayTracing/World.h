/*****************************************************************************
* @File : D:\MyGit\RayTracingManual\RealTimeRayTracing\World.h
* @Author : Wang ZhiHao
* @Date : 2022/3/24 16:49
*****************************************************************************/

#pragma once
#include "Common.h"
#include "CSObject.h"

class World
{
public:
	void Init();
public:
	void Tick(DWORD deltaTime);
	void Render();

public:
	int GetSphereByteWidth();
	int GetSphereStride();

	std::vector<Sphere>& GetSphereVec();

protected:
	void CreateWorld();
	void CreateSphere();

protected:
	std::vector<Sphere> m_VecSphere;
};

