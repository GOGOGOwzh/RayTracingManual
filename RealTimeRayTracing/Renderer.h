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

protected:
	void CompileShadersAndSetLayout();
	void CreateShaderResourse();
	void CreateLayout(ComPtr<ID3D10Blob> vertexShaderBuffer,ComPtr<ID3D10Blob> pixelShaderBuffer);
	void CreateVertexBuffer();
	void CreateShaderSampler();
	void CreateComputeShaderBuffer();

protected:
	ComPtr<ID3D11VertexShader> m_VertextShader;
	ComPtr<ID3D11PixelShader> m_PixelShader;
	ComPtr<ID3D11ComputeShader> m_ComputerShader;

	ComPtr<ID3D11InputLayout> m_VertexLayout;

	ComPtr<ID3D11Buffer> m_VertexBuffer;

	ComPtr<ID3D11SamplerState> m_SamplerWrap;

	ComPtr<ID3D11UnorderedAccessView> m_ResultTexUAV;
	ComPtr<ID3D11ShaderResourceView> m_ResultTexSRV;

	ComPtr<ID3D11Buffer> m_SphereBuffer;
	ComPtr<ID3D11ShaderResourceView> m_SphereSRV;

	ComPtr<ID3D11Buffer> m_TriangleBuffer;
	ComPtr<ID3D11ShaderResourceView> m_TriangleSRV;

	ComPtr<ID3D11Buffer> m_BVHBuffer;
	ComPtr<ID3D11ShaderResourceView> m_BVHSRV;

	ComPtr<ID3D11Buffer> m_CSConstBuffer;
};

