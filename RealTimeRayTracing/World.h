/*****************************************************************************
* @File : D:\MyGit\RayTracingManual\RealTimeRayTracing\World.h
* @Author : Wang ZhiHao
* @Date : 2022/3/24 16:49
*****************************************************************************/

#pragma once
#include "Common.h"

class World
{
public:
	void Init();
public:
	void Tick(DWORD deltaTime);
	void Render();

};

