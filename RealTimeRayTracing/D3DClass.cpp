#include "D3DClass.h"

#include <dxgi1_2.h>
#include <d3dcompiler.h>
#include "GameApp.h"

void D3DClass::Init()
{
	auto hWnd = GameApp::Instance().GetHwnd();
	auto width = GameApp::Instance().GetWidth();
	auto height = GameApp::Instance().GetHeight();

	UINT createDeviceFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
#ifdef _DEBUG
	createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	D3D_FEATURE_LEVEL featureLevels[] =
	{
		D3D_FEATURE_LEVEL_11_1,
	};

	ComPtr<IDXGIFactory2> dxgiFactory;
	ThrowIfFailed(CreateDXGIFactory1(IID_PPV_ARGS(&dxgiFactory)));

	ComPtr<IDXGIAdapter1> dxgiAdapter;
	for (unsigned i = 0; dxgiFactory->EnumAdapters1(i, &dxgiAdapter) != DXGI_ERROR_NOT_FOUND; i++)
	{
		DXGI_ADAPTER_DESC1 desc;
		dxgiAdapter->GetDesc1(&desc);

		//Ìø¹ýÈí¼þadapter
		if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) continue;

		ThrowIfFailed(D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr,
			createDeviceFlags, featureLevels, ARRAYSIZE(featureLevels),
			D3D11_SDK_VERSION, &m_D3DDevice, nullptr, &m_D3DContext));

		printf("DirectX11 Adapter: %ls, VideoMemory:%dM\n", desc.Description, (int)(desc.DedicatedVideoMemory / 1024 / 1024));
		break;
	}

	RECT rect;
	GetClientRect(hWnd, &rect);

	DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
	swapChainDesc.BufferCount = 2;
	swapChainDesc.Width = (UINT)(rect.right - rect.left);
	swapChainDesc.Height = (UINT)(rect.bottom - rect.top);
	swapChainDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	//swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;

	ComPtr<IDXGISwapChain1> swapChain1;
	ThrowIfFailed(dxgiFactory->CreateSwapChainForHwnd(m_D3DDevice.Get(), hWnd, &swapChainDesc, nullptr, nullptr, &swapChain1));
	//½ûÖ¹alt+enterÈ«ÆÁ
	ThrowIfFailed(dxgiFactory->MakeWindowAssociation(hWnd, DXGI_MWA_NO_ALT_ENTER));
	ThrowIfFailed(swapChain1.As(&m_SwapChain));

	// Create a render target view
	ID3D11Texture2D* pBackBuffer = nullptr;
	ThrowIfFailed(m_SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&pBackBuffer)));
	ThrowIfFailed(m_D3DDevice->CreateRenderTargetView(pBackBuffer, nullptr, &m_BackBuffer));
	pBackBuffer->Release();

	m_D3DContext->OMSetRenderTargets(1, &m_BackBuffer, nullptr);

	// Setup the viewport
	D3D11_VIEWPORT viewport;
	viewport.Width = (FLOAT)width;
	viewport.Height = (FLOAT)height;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	m_D3DContext->RSSetViewports(1, &viewport);
}

void D3DClass::BeginRender()
{
	float color[4] = {0};

	m_D3DContext->ClearRenderTargetView(m_BackBuffer, color);
}

void D3DClass::EndRender()
{
	m_SwapChain->Present(1, 0);
}
