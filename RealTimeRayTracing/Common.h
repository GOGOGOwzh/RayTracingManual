/*****************************************************************************
* @File : D:\MyGit\RayTracingManual\RealTimeRayTracing\Common.h
* @Author : Wang ZhiHao
* @Date : 2022/3/24 16:54
*****************************************************************************/
#pragma once

#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <array>
#include <vector>
#include <list>
#include <set>
#include <map>

#include <windows.h>
#include <wrl.h>
#include <d3d11.h>
#include <DirectXMath.h>
#include <directxcolors.h>
#include <functional>

#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "shcore.lib")
#pragma comment(lib, "imm32.lib")

template <class T>
using ComPtr = Microsoft::WRL::ComPtr<T>;

inline void ThrowIfFailed(HRESULT hr)
{
	if (FAILED(hr))
	{
		char str[64] = {};
		sprintf_s(str, "Failure with HRESULT of %08X", static_cast<unsigned int>(hr));
		throw std::runtime_error(str);
	}
}