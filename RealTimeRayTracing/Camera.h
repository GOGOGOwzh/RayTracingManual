/*****************************************************************************
* @File : D:\MyGit\RayTracingManual\RealTimeRayTracing\Camera.h
* @Author : Wang ZhiHao
* @Date : 2022/3/24 16:47
*****************************************************************************/
#pragma once
#include "Common.h"

class Camera
{
public:
	void Init();

public:
	void Tick(DWORD deltaTime);
	void Render();

};

