/*****************************************************************************
* @File : D:\MyGit\RayTracingManual\RealTimeRayTracing\GameApp.h
* @Author : Wang ZhiHao
* @Date : 2022/3/24 16:45
*****************************************************************************/

#pragma once
#include "Common.h"
#include "Mouse.h"
#include "Keyboard.h"

class Camera;
class World;
class D3DClass;
class Renderer;

class GameApp
{
private:
	GameApp();
	GameApp(const GameApp& app);

public:
	static GameApp& Instance();

	void Init();
	void Run();
	void Tick(DWORD deltaTime);
	void Render();

public:
	std::shared_ptr<Camera> GetCamera() {
		return m_Camera;
	}

	std::shared_ptr<World> GetWorld() {
		return m_World;
	}

	std::shared_ptr<D3DClass> GetD3DClass() {
		return m_D3DClass;
	}

	float GetWidth() {
		return m_Width;
	}

	float GetHeight() {
		return m_Height;
	}
	HWND GetHwnd(){
		return m_Hwnd;
	}

	int GetCurTimestamp();

	void ResetSampleCount();
protected:
	void InitWindows();

protected:

	std::shared_ptr<Camera> m_Camera;
	std::shared_ptr<World> m_World;
	std::shared_ptr<D3DClass> m_D3DClass;
	std::shared_ptr<Renderer> m_Renderer;

protected:
	float  m_Width = 784.0f;
	float  m_Height = 784.0f;

	std::int64_t m_PrevTime = 0;

	HWND m_Hwnd = nullptr;
	DirectX::Mouse m_Mouse;
	DirectX::Keyboard m_Keyboard;
};

