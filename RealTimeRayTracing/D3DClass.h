/*****************************************************************************
* @File : D:\MyGit\RayTracingManual\RealTimeRayTracing\D3DClass.h
* @Author : Wang ZhiHao
* @Date : 2022/3/24 16:48
*****************************************************************************/

#pragma once
#include "Common.h"

class D3DClass
{
public:
	void Init();
	void BeginRender();
	void EndRender();

public:
	ID3D11Device* GetD3DDevice()
	{
		return m_D3DDevice.Get();
	}
	ID3D11DeviceContext* GetDeviceContext()
	{
		return m_D3DContext.Get();
	}

	ID3D11RenderTargetView* GetBackBuffer()
	{
		return m_BackBuffer;
	}

protected:

	ComPtr<ID3D11Device> m_D3DDevice;
	ComPtr<ID3D11DeviceContext> m_D3DContext;
	ComPtr<IDXGISwapChain> m_SwapChain;
	ID3D11RenderTargetView* m_BackBuffer = nullptr;
};

