/*****************************************************************************
* @File : D:\MyGit\RayTracingManual\RayTracingManual\WhittedStyleRayTracing\Camera.h
* @Author : Wang ZhiHao
* @Date : 2021/9/14 16:15
*****************************************************************************/

#pragma once

#include "Ray.h"

class Camera :   public Object
{
public:
	/// <summary>
	/// Ĭ����Ϊ������10
	/// </summary>
	/// <param name="pos">���λ��</param>
	/// <param name="lookdir">����</param>
	/// <param name="fov">�ӽ�</param>
	/// <param name="aspect">��߱�</param>
	/// <param name="distance">����</param>
	Camera(const Point3& pos, const Vector3f & lookdir,const Vector3f & up, float fov = 90.0, float aspect =1.0, float distance=10.0):Object(TYPE_CAMERA)
	{
		m_Pos = pos;
		m_Dir = lookdir;
		m_Fov = fov;
		m_Aspect = aspect;
		m_Up = up;
		m_FocusDis = distance;

		CalcCameraInfo();
	}
	virtual ~Camera() = default;

public:
	void CalcCameraInfo();
	Ray GetRay(float u, float v) const;

protected:
	Point3 m_Pos;
	Point3 m_LeftLowCorner;
	Vector3f m_Dir;
	Vector3f m_Up;
	Vector3f m_Horizon;
	Vector3f m_Vertical;

	float m_Fov = 0;
	float m_Aspect = 0;
	float m_FocusDis = 10;
};

