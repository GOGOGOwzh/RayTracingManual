/*****************************************************************************
* @File : D:\Games101\RayTracingManual\RayTracingManual\WhittedStyleRayTracing\Ray.h
* @Author : Wang ZhiHao
* @Date : 2021/9/14 15:50
*****************************************************************************/

#pragma once

#include "Object.h"

class Ray :public Object
{
public:
	Point3 m_orgPos;
	Vector3f m_dir;
	float m_time = 0;
public:
	Ray(const Point3& pos, const Vector3f& dir):Object(TYPE_RAY),m_orgPos(pos), m_dir(dir){}
	Ray(const Point3& pos, const Vector3f& dir, const float time) :Ray(pos,dir){
		m_time = time;
	}
	virtual ~Ray() = default;

	const Point3& GetOrgPos()const { return m_orgPos; }
	const Vector3f& GetDir()const { return m_dir; }
	const float GetTime()const { return m_time; }

	Point3 At(float t) {
		return m_orgPos + t * m_dir;
	}

	Point3 AtTime()
	{
		return m_orgPos + m_time * m_dir;
	}
};
