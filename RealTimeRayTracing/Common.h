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
#include "SimpleMath.h"
#include <directxcolors.h>
#include <functional>
#include <d3dcompiler.h>

#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "shcore.lib")
#pragma comment(lib, "imm32.lib")
#pragma comment( lib, "dxguid.lib")

#define M_PI 3.14159265358979323846

#define Self_Max(a, b) (((a) > (b)) ? (a) : (b))
#define Self_Min(a, b) (((a) < (b)) ? (a) : (b))

using Vec2 = DirectX::SimpleMath::Vector2;
using Vec3 = DirectX::SimpleMath::Vector3;
using Vec4 = DirectX::SimpleMath::Vector4;
using Mat4x4 = DirectX::SimpleMath::Matrix;

const Vec3 G_AxisY = Vec3(0, 1, 0);

inline float Degrees_to_Radians(float degrees)
{
	return degrees * M_PI / 180;
}

inline float Radians_to_Degrees(float radians)
{
	return radians * 180 / M_PI;
}

inline Vec3 Max(const Vec3& v1, const Vec3& v2)
{
	return Vec3(Self_Max(v1.x, v2.x), Self_Max(v1.y, v2.y), Self_Max(v1.z, v2.z));
}

inline Vec3 Min(const Vec3& v1, const Vec3& v2)
{
	return Vec3(Self_Min(v1.x, v2.x), Self_Min(v1.y, v2.y), Self_Min(v1.z, v2.z));
}

inline void ThrowIfFailed(HRESULT hr)
{
	if (FAILED(hr))
	{
		char str[64] = {};
		sprintf_s(str, "Failure with HRESULT of %08X", static_cast<unsigned int>(hr));
		throw std::runtime_error(str);
	}
}

#define SAFE_RELEASE(p) { if ((p)) { (p)->Release(); (p) = nullptr; } }

inline HRESULT CreateShaderFromFile(const WCHAR* csoFileNameInOut, const WCHAR* hlslFileName,
	LPCSTR entryPoint, LPCSTR shaderModel, ID3DBlob** ppBlobOut)
{
	HRESULT hr = S_OK;

	// 寻找是否有已经编译好的顶点着色器
	if (csoFileNameInOut && D3DReadFileToBlob(csoFileNameInOut, ppBlobOut) == S_OK)
	{
		return hr;
	}
	else
	{
		DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#ifdef _DEBUG
		// 设置 D3DCOMPILE_DEBUG 标志用于获取着色器调试信息。该标志可以提升调试体验，
		// 但仍然允许着色器进行优化操作
		dwShaderFlags |= D3DCOMPILE_DEBUG;

		// 在Debug环境下禁用优化以避免出现一些不合理的情况
		dwShaderFlags |= D3DCOMPILE_SKIP_OPTIMIZATION;
#endif
		ID3DBlob* errorBlob = nullptr;
		hr = D3DCompileFromFile(hlslFileName, nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, entryPoint, shaderModel,
			dwShaderFlags, 0, ppBlobOut, &errorBlob);
		if (FAILED(hr))
		{
			if (errorBlob != nullptr)
			{
				OutputDebugStringA(reinterpret_cast<const char*>(errorBlob->GetBufferPointer()));
			}
			SAFE_RELEASE(errorBlob);
			return hr;
		}

		// 若指定了输出文件名，则将着色器二进制信息输出
		if (csoFileNameInOut)
		{
			return D3DWriteBlobToFile(*ppBlobOut, csoFileNameInOut, FALSE);
		}
	}

	return hr;
}

template <class T>
using ComPtr = Microsoft::WRL::ComPtr<T>;

template <class T>
inline void ClampT(T& x, const T& min, const T& max)
{
	if (x < min)x = min;
	if (x > max)x = max;
}
