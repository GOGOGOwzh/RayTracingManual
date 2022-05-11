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

	void SetSize(float width, float height)
	{
		m_Width = width;
		m_Height = height;
	}

	void SetPosition(const Vec3& pos) {
		m_Position = pos;
	}

	void SetFov(const float& fov) {
		m_Fov = fov;
	}

	void SetUp(const Vec3& up) {
		m_Up = up;
	}

	Mat4x4 GetVPInvert();

	Vec3 GetCameraPosition();

protected:
	void UpdateProjectionMat();
	void UpdateViewMat();
	void UpdateCameraCtrl(DWORD deltaTime);
	void DoYaw(DWORD deltaTime);
	void DoPitch(DWORD deltaTime);

	Vec3 GetDirFromYawPitch();

protected:
	bool m_FirstTick = true;
	float m_LastMousePosX = 0.0f;
	float m_LastMousePosY = 0.0f;

	float m_Yaw = 90.0f;
	float m_Pitch = 0.0f;

	float m_OrgYaw = 90.0f;
	float m_OrgPitch = 0.0f;

	float m_MoveSpeed = 10.0f;
	float m_YawPitchSpeed = 1.0f;

	float m_NearPlane = 0.001f;
	float m_FarPlane = 1000.0f;
	float m_Width;
	float m_Height;
	float m_Fov = 40.0f;

	Vec3 m_Up = Vec3(0, 1, 0);
	Vec3 m_Position = Vec3(278, 273, -130);
	Vec3 m_Dir = Vec3(0, 0, 1);

	Vec3 m_OrgUp = Vec3(0, 1, 0);
	Vec3 m_OrgPosition = Vec3(278, 273, -130);
	Vec3 m_OrgDir = Vec3(0, 0, 1);

	Mat4x4 m_ViewMat;
	Mat4x4 m_ProjectionMat;
};

