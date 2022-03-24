/*****************************************************************************
* @File : D:\MyGit\RayTracingManual\RealTimeRayTracing\Render.h
* @Author : Wang ZhiHao
* @Date : 2022/3/24 17:52
*****************************************************************************/

#pragma once
#include "Common.h"

class Renderer
{
public:
	void Init();
public:
	void Tick(DWORD deltaTime);
	void Render();
};

