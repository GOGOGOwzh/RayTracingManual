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
	/// 默认认为焦距是10
	/// </summary>
	/// <param name="pos">相机位置</param>
	/// <param name="lookdir">朝向</param>
	/// <param name="fov">视角</param>
	/// <param name="aspect">宽高比</param>
	/// <param name="distance">焦距</param>
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

