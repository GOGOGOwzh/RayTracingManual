#include "Renderer.h"
#include "GameApp.h"
#include "D3DClass.h"
#include "World.h"
#include "Camera.h"

void Renderer::Init()
{
	CompileShadersAndSetLayout();
	CreateVertexBuffer();
	CreateShaderResourse();
	CreateShaderSampler();
	CreateBlendState();
	CreateComputeShaderBuffer();
}

void Renderer::Tick(DWORD deltaTime)
{

}

void Renderer::Render()
{
	auto d3dClass = GameApp::Instance().GetD3DClass();
	auto cameraClass = GameApp::Instance().GetCamera();
	int timestamp = GameApp::Instance().GetCurTimestamp();

	//d3dClass->BeginRender();

	auto d3dContext = d3dClass->GetDeviceContext();
	auto d3dDevice = d3dClass->GetD3DDevice();

	float width = GameApp::Instance().GetWidth();
	float height = GameApp::Instance().GetHeight();

	//设置CS
	d3dContext->CSSetShader(m_ComputerShader.Get(), nullptr, 0);
	//设置无序视图，这个试图也是一个ShaderResourceView,可以直接用于PixelShader
	d3dContext->CSSetUnorderedAccessViews(0, 1, m_ResultTexUAV.GetAddressOf(), nullptr);
	
	//设置CBuffer
	auto vpInvert = cameraClass->GetVPInvert();
	auto cameraPos = cameraClass->GetCameraPosition();
	CB cb = { vpInvert,cameraPos,m_iSampleCount,m_iSampleCount };
	d3dContext->UpdateSubresource(m_CSConstBuffer.Get(), 0, nullptr, &cb, 0, 0);
	d3dContext->CSSetConstantBuffers(0, 1, m_CSConstBuffer.GetAddressOf());

	//设置Sphere的SRV
	d3dContext->CSSetShaderResources(0, 1, m_SphereSRV.GetAddressOf());

	//设置Triangle的SRV
	d3dContext->CSSetShaderResources(1, 1, m_TriangleSRV.GetAddressOf());

	//设置BVH的SRV
	d3dContext->CSSetShaderResources(2, 1, m_BVHSRV.GetAddressOf());

	//设置LightTriangle的SRV
	d3dContext->CSSetShaderResources(3, 1, m_LightTriangleSRV.GetAddressOf());

	//运行CS
	d3dContext->Dispatch((UINT)ceilf(width / 16.0f), (UINT)ceilf(height / 16.0f), 1);

	ID3D11UnorderedAccessView* nullUAV = nullptr;
	d3dContext->CSSetUnorderedAccessViews(0, 1, &nullUAV, nullptr);

	d3dContext->OMSetBlendState(m_BlendTransparent.Get(), nullptr, 0xFFFFFFFF);
	d3dContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	d3dContext->IASetInputLayout(m_VertexLayout.Get());

	UINT stride = sizeof(float) * 4;
	UINT offset = 0;
	d3dContext->IASetVertexBuffers(0, 1, m_VertexBuffer.GetAddressOf(), &stride, &offset);

	d3dContext->PSSetConstantBuffers(0, 1, m_CSConstBuffer.GetAddressOf());
	d3dContext->PSSetShaderResources(0, 1, m_ResultTexSRV.GetAddressOf());
	d3dContext->VSSetShader(m_VertextShader.Get(), nullptr, 0);
	d3dContext->PSSetShader(m_PixelShader.Get(), nullptr, 0);
	d3dContext->PSSetSamplers(0, 1, m_SamplerWrap.GetAddressOf());
	d3dContext->Draw(6, 0);

	ID3D11ShaderResourceView* nullSRV = nullptr;
	d3dContext->PSSetShaderResources(0, 1, &nullSRV);

	m_iSampleCount++;

	d3dClass->EndRender();
}

void Renderer::CompileShadersAndSetLayout()
{
	auto d3dDevice = GameApp::Instance().GetD3DClass()->GetD3DDevice();
	
	const WCHAR* computeShaderFilePath = L"Shaders\\ComputeShader.hlsl";
	const WCHAR* vertextShaderFilePath = L"Shaders\\VertexShader.hlsl";
	const WCHAR* pixelShaderFilePath   = L"Shaders\\PixelShader.hlsl";

	ComPtr<ID3D10Blob> computeShaderBuffer;
	ComPtr<ID3D10Blob> vertexShaderBuffer;
	ComPtr<ID3D10Blob> pixelShaderBuffer;

	ThrowIfFailed(CreateShaderFromFile(nullptr, computeShaderFilePath, "CS_main", "cs_5_0", computeShaderBuffer.GetAddressOf()));
	ThrowIfFailed(d3dDevice->CreateComputeShader(computeShaderBuffer->GetBufferPointer(), computeShaderBuffer->GetBufferSize(), nullptr, m_ComputerShader.GetAddressOf()));

	ThrowIfFailed(CreateShaderFromFile(nullptr, vertextShaderFilePath, "VS_main", "vs_5_0", vertexShaderBuffer.GetAddressOf()));
	ThrowIfFailed(d3dDevice->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(),vertexShaderBuffer->GetBufferSize(),nullptr,m_VertextShader.GetAddressOf()));

	ThrowIfFailed(CreateShaderFromFile(nullptr, pixelShaderFilePath, "PS_main", "ps_5_0", pixelShaderBuffer.GetAddressOf()));
	ThrowIfFailed(d3dDevice->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), nullptr, m_PixelShader.GetAddressOf()));

	CreateLayout(vertexShaderBuffer, pixelShaderBuffer);
}

void Renderer::CreateShaderResourse()
{
	auto d3dDevice = GameApp::Instance().GetD3DClass()->GetD3DDevice();

	D3D11_TEXTURE2D_DESC texDesc;
	texDesc.Width = GameApp::Instance().GetWidth();
	texDesc.Height = GameApp::Instance().GetHeight();
	texDesc.MipLevels = 1;
	texDesc.ArraySize = 1;
	texDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	texDesc.SampleDesc.Count = 1;
	texDesc.SampleDesc.Quality = 0;
	texDesc.Usage = D3D11_USAGE_DEFAULT;
	texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
	texDesc.CPUAccessFlags = 0;
	texDesc.MiscFlags = 0;

	ComPtr<ID3D11Texture2D> texture = nullptr;
	ThrowIfFailed(d3dDevice->CreateTexture2D(&texDesc, 0, texture.GetAddressOf()));

	D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc;
	uavDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	uavDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
	uavDesc.Texture2D.MipSlice = 0;
	ThrowIfFailed(d3dDevice->CreateUnorderedAccessView(texture.Get(), &uavDesc, &m_ResultTexUAV));

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	srvDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = 1;
	ThrowIfFailed(d3dDevice->CreateShaderResourceView(texture.Get(), &srvDesc, &m_ResultTexSRV));

}

void Renderer::CreateLayout(ComPtr<ID3D10Blob> vertexShaderBuffer, ComPtr<ID3D10Blob> pixelShaderBuffer)
{
	auto d3dDevice = GameApp::Instance().GetD3DClass()->GetD3DDevice();

	D3D11_INPUT_ELEMENT_DESC layout[] = {
		{"POSITION",0,DXGI_FORMAT_R32G32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0},
		{"TEXCOORD",0,DXGI_FORMAT_R32G32_FLOAT,0,8,D3D11_INPUT_PER_VERTEX_DATA,0}
	};

	auto elementCount = sizeof(layout) / sizeof(layout[0]);
	ThrowIfFailed(d3dDevice->CreateInputLayout(layout, elementCount, vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), m_VertexLayout.GetAddressOf()));
}

void Renderer::CreateVertexBuffer()
{
	auto d3dDevice = GameApp::Instance().GetD3DClass()->GetD3DDevice();

	float vertices[] =
	{
		-1.0f, 1.0f, 0.0f, 0.0f,
		1.0f, 1.0f, 1.0f, 0.0f,
		1.0f, -1.0f, 1.0f, 1.0f,
		-1.0f, 1.0f, 0.0f, 0.0f,
		1.0f, -1.0f, 1.0f, 1.0f,
		-1.0f, -1.0f, 0.0f, 1.0f,
	};

	D3D11_BUFFER_DESC vertexBuffDesc = {};
	vertexBuffDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBuffDesc.ByteWidth = sizeof(vertices);
	vertexBuffDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBuffDesc.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA vertexData = {};
	vertexData.pSysMem = vertices;

	ThrowIfFailed(d3dDevice->CreateBuffer(&vertexBuffDesc, &vertexData, m_VertexBuffer.GetAddressOf()));

#if defined(_DEBUG)
	m_VertexBuffer->SetPrivateData(WKPDID_D3DDebugObjectName, sizeof("Vertex") - 1, "Vertex");
#endif
}

void Renderer::CreateShaderSampler()
{
	auto d3dDevice = GameApp::Instance().GetD3DClass()->GetD3DDevice();

	D3D11_SAMPLER_DESC samplerDesc = {};
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.BorderColor[0] = 0;
	samplerDesc.BorderColor[1] = 0;
	samplerDesc.BorderColor[2] = 0;
	samplerDesc.BorderColor[3] = 0;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	ThrowIfFailed(d3dDevice->CreateSamplerState(&samplerDesc, m_SamplerWrap.GetAddressOf()));
}

void Renderer::CreateBlendState()
{
	auto d3dDevice = GameApp::Instance().GetD3DClass()->GetD3DDevice();

	D3D11_BLEND_DESC blendDesc = {};
	auto& rtDesc = blendDesc.RenderTarget[0];
	blendDesc.AlphaToCoverageEnable = false;
	blendDesc.IndependentBlendEnable = false;
	rtDesc.BlendEnable = true;
	rtDesc.SrcBlend = D3D11_BLEND_SRC_ALPHA;
	rtDesc.DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	rtDesc.BlendOp = D3D11_BLEND_OP_ADD;
	rtDesc.SrcBlendAlpha = D3D11_BLEND_ONE;
	rtDesc.DestBlendAlpha = D3D11_BLEND_ZERO;
	rtDesc.BlendOpAlpha = D3D11_BLEND_OP_ADD;
	rtDesc.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	ThrowIfFailed(d3dDevice->CreateBlendState(&blendDesc, m_BlendTransparent.GetAddressOf()));
}

void Renderer::CreateComputeShaderBuffer()
{
	auto d3dDevice = GameApp::Instance().GetD3DClass()->GetD3DDevice();
	auto world = GameApp::Instance().GetWorld();

	//Const Buffer 常量缓冲区
	{
		D3D11_BUFFER_DESC desc = {};
		desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.ByteWidth = sizeof(CB);
		desc.StructureByteStride = 0;

		ThrowIfFailed(d3dDevice->CreateBuffer(&desc, nullptr, m_CSConstBuffer.GetAddressOf()));
#if defined(_DEBUG)
		m_CSConstBuffer->SetPrivateData(WKPDID_D3DDebugObjectName, sizeof("CBufer") - 1, "CBufer");
#endif
	}

	//Sphere
	{
		D3D11_BUFFER_DESC desc = {};
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED; //这个地方决定使用了structuredBuffer
		desc.ByteWidth = world->GetSphereByteWidth();
		desc.StructureByteStride = world->GetSphereStride();
		D3D11_SUBRESOURCE_DATA data;
		data.pSysMem = world->GetSphereVec().data();

		ThrowIfFailed(d3dDevice->CreateBuffer(&desc, &data, m_SphereBuffer.GetAddressOf()));

#if defined(_DEBUG)
		m_SphereBuffer->SetPrivateData(WKPDID_D3DDebugObjectName, sizeof("Spheres") - 1, "Spheres");
#endif			
		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
		srvDesc.Format = DXGI_FORMAT_UNKNOWN;
		srvDesc.Buffer.FirstElement = 0;
		srvDesc.Buffer.NumElements = world->GetSphereVec().size();
		ThrowIfFailed(d3dDevice->CreateShaderResourceView(m_SphereBuffer.Get(),&srvDesc,m_SphereSRV.GetAddressOf()));
	}

	//Triangle
	{
		D3D11_BUFFER_DESC desc = {};
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED; //这个地方决定使用了structuredBuffer

		desc.ByteWidth = world->GetTriangleByteWidth();
		desc.StructureByteStride = world->GetTriangleStride();
		D3D11_SUBRESOURCE_DATA data;
		data.pSysMem = world->GetTriangleVec().data();

		ThrowIfFailed(d3dDevice->CreateBuffer(&desc, &data, m_TriangleBuffer.GetAddressOf()));

#if defined(_DEBUG)
		m_TriangleBuffer->SetPrivateData(WKPDID_D3DDebugObjectName, sizeof("Triangles") - 1, "Triangles");
#endif			
		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
		srvDesc.Format = DXGI_FORMAT_UNKNOWN;
		srvDesc.Buffer.FirstElement = 0;
		srvDesc.Buffer.NumElements = world->GetTriangleVec().size();
		ThrowIfFailed(d3dDevice->CreateShaderResourceView(m_TriangleBuffer.Get(), &srvDesc, m_TriangleSRV.GetAddressOf()));
	}

	//LightTriangle
	{
		D3D11_BUFFER_DESC desc = {};
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED; //这个地方决定使用了structuredBuffer

		desc.ByteWidth = world->GetLightTriangleByteWidth();
		desc.StructureByteStride = world->GetTriangleStride();
		D3D11_SUBRESOURCE_DATA data;
		data.pSysMem = world->GetLightTriangleVec().data();

		ThrowIfFailed(d3dDevice->CreateBuffer(&desc, &data, m_LightTriangleBuffer.GetAddressOf()));

#if defined(_DEBUG)
		m_LightTriangleBuffer->SetPrivateData(WKPDID_D3DDebugObjectName, sizeof("LightTriangles") - 1, "LightTriangles");
#endif			
		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
		srvDesc.Format = DXGI_FORMAT_UNKNOWN;
		srvDesc.Buffer.FirstElement = 0;
		srvDesc.Buffer.NumElements = world->GetLightTriangleVec().size();
		ThrowIfFailed(d3dDevice->CreateShaderResourceView(m_LightTriangleBuffer.Get(), &srvDesc, m_LightTriangleSRV.GetAddressOf()));
	}

	//BVH
	{
		D3D11_BUFFER_DESC desc = {};
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED; //这个地方决定使用了structuredBuffer

		desc.ByteWidth = world->GetBVHByteWidth();
		desc.StructureByteStride = world->GetBVHStride();
		D3D11_SUBRESOURCE_DATA data;
		data.pSysMem = world->GetBVHVec().data();

		ThrowIfFailed(d3dDevice->CreateBuffer(&desc, &data, m_BVHBuffer.GetAddressOf()));

#if defined(_DEBUG)
		m_BVHBuffer->SetPrivateData(WKPDID_D3DDebugObjectName, sizeof("BVH") - 1, "BVH");
#endif			
		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
		srvDesc.Format = DXGI_FORMAT_UNKNOWN;
		srvDesc.Buffer.FirstElement = 0;
		srvDesc.Buffer.NumElements = world->GetBVHVec().size();
		ThrowIfFailed(d3dDevice->CreateShaderResourceView(m_BVHBuffer.Get(), &srvDesc, m_BVHSRV.GetAddressOf()));
	}
}

