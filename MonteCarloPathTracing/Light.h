/*****************************************************************************
* @File : D:\MyGit\RayTracingManual\RayTracingManual\WhittedStyleRayTracing\Light.h
* @Author : Wang ZhiHao
* @Date : 2022/1/20 15:00
*****************************************************************************/

#pragma once
#include "Object.h"
class Light : public Object
{
	Point3 m_pos;
	Color3 m_color = Color3(1);
	float m_intensity = 0;
public:
	Light() :Object(TYPE_LIGHT) {}
	Light(const Point3& pos, const float& intensity) :Light()
	{
		m_pos = pos;
		m_intensity = intensity;
	}
	virtual ~Light() = default;

public:
	Point3 GetPos() {
		return m_pos;
	}
	float GetIntensity() {
		return m_intensity;
	}
	Color3 GetColor() {
		return m_color;
	}

	void SetColor(const Color3 & color) 
	{
		m_color = color;
	}
};

