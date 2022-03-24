#include "GameApp.h"
#include "D3DClass.h"
#include "Renderer.h"
#include "Camera.h"
#include "World.h"

#define CLASS_NAME L"RealTimeRayTrcing"

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_ACTIVATEAPP:
	{
		DirectX::Mouse::ProcessMessage(message, wParam, lParam);
		DirectX::Keyboard::ProcessMessage(message, wParam, lParam);
		break;
	}
	case WM_INPUT:
	case WM_MOUSEMOVE:
	case WM_LBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_RBUTTONDOWN:
	case WM_RBUTTONUP:
	case WM_MBUTTONDOWN:
	case WM_MBUTTONUP:
	case WM_MOUSEWHEEL:
	case WM_XBUTTONDOWN:
	case WM_XBUTTONUP:
	case WM_MOUSEHOVER:
		DirectX::Mouse::ProcessMessage(message, wParam, lParam);
		break;
	case WM_KEYDOWN:
	case WM_SYSKEYDOWN:
	case WM_KEYUP:
	case WM_SYSKEYUP:
		DirectX::Keyboard::ProcessMessage(message, wParam, lParam);
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}

	return 0;
}

GameApp::GameApp()
{

}

GameApp::GameApp(const GameApp& app)
{

}

GameApp& GameApp::Instance()
{
	static GameApp g_instance;
	return g_instance;
}

void GameApp::Init()
{
	InitWindows();

	m_D3DClass = std::make_shared<D3DClass>();
	m_D3DClass->Init();

	m_Renderer = std::make_shared<Renderer>();
	m_Renderer->Init();

	m_Camera = std::make_shared<Camera>();
	m_Camera->Init();

	m_World = std::make_shared<World>();
	m_World->Init();
}

void GameApp::Run()
{
	ShowWindow(m_Hwnd, SW_SHOW);

	int64_t countsPerSec;
	QueryPerformanceFrequency((LARGE_INTEGER*)&countsPerSec);
	double secondsPerCount = 1.0 / countsPerSec;
	QueryPerformanceCounter((LARGE_INTEGER*)&m_PrevTime);

	// Main message loop
	MSG msg = { 0 };
	while (WM_QUIT != msg.message)
	{
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			int64_t currentTime;
			QueryPerformanceCounter((LARGE_INTEGER*)&currentTime);
			double deltaTime = (currentTime - m_PrevTime) * secondsPerCount;

			Tick(deltaTime);
			Render();
		}
	}
}

void GameApp::Tick(DWORD deltaTime)
{
	m_Camera->Tick(deltaTime);
	m_World->Tick(deltaTime);
	m_Renderer->Tick(deltaTime);
}

void GameApp::Render()
{
	m_Camera->Render();
	m_World->Render();
	m_Renderer->Render();
}

void GameApp::InitWindows()
{
	// Register class
	WNDCLASSEX wcex;
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = nullptr;
	wcex.hIcon = (HICON)LoadImageA(nullptr, "icon.ico", IMAGE_ICON, 0, 0, LR_LOADFROMFILE | LR_DEFAULTSIZE | LR_SHARED);
	wcex.hIconSm = wcex.hIcon;
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = nullptr;
	wcex.lpszClassName = CLASS_NAME;
	if (!RegisterClassEx(&wcex))
	{
		MessageBoxA(0, "RegisterClassEx Failed.", "Error", MB_OK);
		exit(0);
	}

	// Create window
	RECT rc = { 0, 0, (LONG)m_Width, (LONG)m_Height };
	AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);
	m_Hwnd = CreateWindowEx(WS_EX_ACCEPTFILES, wcex.lpszClassName, CLASS_NAME,
		WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
		rc.right - rc.left, rc.bottom - rc.top, nullptr, nullptr, nullptr, this);
	if (!m_Hwnd)
	{
		MessageBoxA(0, "CreateWindowEx Failed.", "Error", MB_OK);
		exit(0);
	}

	m_Mouse.SetWindow(m_Hwnd);
}
