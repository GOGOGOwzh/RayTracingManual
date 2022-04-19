#include "Camera.h"
#include "Mouse.h"
#include "Keyboard.h"
#include "GameApp.h"

static float const DELTATIME_MUL = 0.01f;
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
	DirectX::XMVECTOR pos = DirectX::XMLoadFloat3(&m_Position);
	DirectX::XMVECTOR up = DirectX::XMLoadFloat3(&m_Up);
	DirectX::XMVECTOR dir = DirectX::XMLoadFloat3(&m_Dir);

	m_ViewMat = DirectX::XMMatrixLookToRH(pos, dir, up);
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

	if (m_FirstTick)
	{
		m_FirstTick = false;
	}

	if (Keyboard::Get().GetState().R) //Reset
	{
		m_Up = Vec3(0, 1, 0);
		m_Position = Vec3(0, 0, 5);
		m_Dir = Vec3(0, 0, -1);

		m_Pitch = 0;
		m_Yaw = 0;
	}
}

void Camera::DoYaw(DWORD deltaTime)
{
	using namespace DirectX;
	float posX = Mouse::Get().GetState().x;
	bool bRBDown = Mouse::Get().GetState().rightButton;

	if (m_FirstTick)
	{
		m_LastMousePosX = posX;
		return;
	}

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

	if (m_FirstTick)
	{
		m_LastMousePosY = posY;
		return;
	}

	if (bRBDown)
	{
		auto pitch = (m_LastMousePosY - posY) * m_YawPitchSpeed * DELTATIME_MUL * deltaTime;
		m_Pitch += pitch;
		ClampT(m_Pitch, -89.0f, 89.0f);
	}

	m_LastMousePosY = posY;
}

Vec3 Camera::GetDirFromYawPitch()
{
	float theta = Degrees_to_Radians(m_Pitch);
	float phi = Degrees_to_Radians(m_Yaw);
	float rad = 1.0f;

	//�����൱�ڰ��������껻����Y�ᳯ�ϵ���ʽ
	//thetaΪX,Zƽ����rad�ļн�
	//phiΪrad��X,Zƽ��ͶӰproject��X��ļн�
	float project = rad * std::cosf(theta);

	Vec3 dir;
	dir.y = rad * std::sinf(theta);
	dir.x = project * std::cosf(phi);
	dir.z = project * std::sinf(phi);

	dir.Normalize();
	return dir;
}
