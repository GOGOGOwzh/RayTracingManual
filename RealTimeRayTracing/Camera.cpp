#include "Camera.h"
#include "Mouse.h"
#include "Keyboard.h"
#include "GameApp.h"

static float const DELTATIME_MUL = 1 / 65535.0f;
void Camera::Init()
{
	m_Width = GameApp::Instance().GetWidth();
	m_Height = GameApp::Instance().GetHeight();
}

void Camera::Tick(DWORD deltaTime)
{
	UpdateCameraCtrl(deltaTime);
	UpdateViewMat();
	UpdateProjectionMat();
}

void Camera::Render()
{

}

Mat4x4 Camera::GetVPInvert()
{
	auto vpMat = m_ViewMat * m_ProjectionMat;
	return vpMat.Invert();
}

Vec3 Camera::GetCameraPosition()
{
	return m_Position;
}

void Camera::UpdateProjectionMat()
{
	m_ProjectionMat = DirectX::XMMatrixPerspectiveFovRH(m_Fov, m_Width / m_Height, m_NearPlane, m_FarPlane);
}

void Camera::UpdateViewMat()
{
	DirectX::XMVECTOR eye = DirectX::XMLoadFloat3(&m_Position);
	DirectX::XMVECTOR up = DirectX::XMLoadFloat3(&m_Up);
	DirectX::XMVECTOR lookat = DirectX::XMLoadFloat3(&m_Target);

	m_ViewMat = DirectX::XMMatrixLookAtRH(eye, lookat, up);
}

void Camera::UpdateCameraCtrl(DWORD deltaTime)
{
	using namespace DirectX;
	DoYaw(deltaTime); DoPitch(deltaTime);
	m_Dir = GetDirFromYawPitch();

	Vec3 rightVec = m_Dir.Cross(G_AxisY);
	rightVec.Normalize();

	m_Up = rightVec.Cross(m_Dir);
	m_Up.Normalize();

	float speed = m_MoveSpeed * deltaTime * 0.001f;
	if (Keyboard::Get().GetState().A) //left
	{
		m_Position -= rightVec * speed;
	}
	else if (Keyboard::Get().GetState().D)//right
	{
		m_Position += rightVec * speed;
	}

	if (Keyboard::Get().GetState().W) //forward
	{
		m_Position += m_Dir * speed;
	}
	else if (Keyboard::Get().GetState().S)//back
	{
		m_Position -= m_Dir * speed;
	}
}

void Camera::DoYaw(DWORD deltaTime)
{
	using namespace DirectX;
	float posX = Mouse::Get().GetState().x;
	bool bRBDown = Mouse::Get().GetState().rightButton;

	if (bRBDown)
	{
		auto yaw = (m_LastMousePosX - posX) * m_YawPitchSpeed * DELTATIME_MUL * deltaTime;
		m_Yaw += yaw;
	}

	m_LastMousePosX = posX;
}

void Camera::DoPitch(DWORD deltaTime)
{
	using namespace DirectX;
	float posY = Mouse::Get().GetState().y;
	bool bRBDown = Mouse::Get().GetState().rightButton;

	if (bRBDown)
	{
		auto pitch = (m_LastMousePosY - posY) * m_YawPitchSpeed * DELTATIME_MUL * deltaTime;
		m_Pitch += pitch;
	}

	m_LastMousePosY = posY;
}

Vec3 Camera::GetDirFromYawPitch()
{
	float yaw = Degrees_to_Radians(m_Yaw);
	float pitch = Degrees_to_Radians(m_Pitch);
	float rad = 1.0f;
	float project = rad * std::cosf(pitch);

	Vec3 dir;
	dir.y = rad * std::sinf(pitch);
	dir.x = project * std::cosf(yaw);
	dir.z = project * std::sinf(yaw);

	dir.Normalize();
	return dir;
}
