#include "Camera.h"
#include "Global.h"

void Camera::CalcCameraInfo()
{
	
	auto u = CrossProduct(m_Up, -m_Dir).Nor();
	auto v = CrossProduct(-m_Dir, u);

	auto vertical = m_FocusDis * tan(Degrees_to_Radians(m_Fov / 2)) * 2;
	auto horizon = m_Aspect * vertical;

	m_Horizon = u * horizon;
	m_Vertical = v * vertical;

	m_LeftLowCorner = m_Pos - m_Horizon / 2 - m_Vertical / 2 + m_Dir * m_FocusDis;
}

Ray Camera::GetRay(float u, float v) const
{
	return Ray(m_Pos, (m_LeftLowCorner + u * m_Horizon + v * m_Vertical - m_Pos).Nor());
}
