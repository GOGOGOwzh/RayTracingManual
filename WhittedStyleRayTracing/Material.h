/*****************************************************************************
* @File : D:\Games101\RayTracingManual\RayTracingManual\WhittedStyleRayTracing\Material.h
* @Author : Wang ZhiHao
* @Date : 2021/9/14 16:01
*****************************************************************************/

#pragma once
#include "Object.h"

enum MAT_TYPE
{
	MAT_NONE,
	MAT_LAMBERT,
	MAT_REFLECT,
	MAT_REFRACT,
	MAT_REFLECT_REFRACT,
};

class World;
class Material : public Object
{
public:
	char m_matType = MAT_NONE;
	Color3 m_albedo = Color3(1);

	float m_specularExponent = 25.0f;
	float m_specularKS = 0.2;
	float m_diffuseKD = 0.8;

	std::string m_diffuseMap;
	std::string m_bumpMap;

public:
	Material():Object(TYPE_MAT){}
	Material(char type) :Object(TYPE_MAT),m_matType(type) {}
	virtual ~Material() = default;

	char GetMatType() {
		return m_matType;
	}

	void SetAlbedo(const Color3& col) {
		m_albedo = col;
	}

	virtual Color3 Shadring(const std::optional<TraceInfo> & traceInfo,const World * pWorld,int depth) = 0;
};


class MatLambert : public Material
{
public:
	MatLambert():Material(MAT_LAMBERT){}
	MatLambert(const Color3& albedo) :MatLambert()
	{
		m_albedo = albedo;
	}

	virtual Color3 Shadring(const std::optional<TraceInfo>& traceInfo, const World* pWorld, int depth) override;
};

class MatReflect : public Material
{
public:
	MatReflect():Material(MAT_REFLECT){}
	MatReflect(const Color3& albedo) :MatReflect()
	{
		m_albedo = albedo;
	}

	virtual Color3 Shadring(const std::optional<TraceInfo>& traceInfo, const World* pWorld, int depth) override;
};

class MatRefract :public Material
{
public:
	float m_refractivity = 1;
public:
	MatRefract():Material(MAT_REFRACT){}
	MatRefract(const Color3 & albedo,const float refractivity) :MatRefract()
	{
		m_albedo = albedo;
		m_refractivity = refractivity;
	}

	virtual Color3 Shadring(const std::optional<TraceInfo>& traceInfo, const World* pWorld, int depth) override
	{
		return m_albedo;
	}
};

class MatReflectRefract :public Material
{
public:
	float m_refractivity = 1;
public:
	MatReflectRefract() :Material(MAT_REFLECT_REFRACT) {}
	MatReflectRefract(const Color3& albedo, const float refractivity) :MatReflectRefract()
	{
		m_albedo = albedo;
		m_refractivity = refractivity;
	}

	virtual Color3 Shadring(const std::optional<TraceInfo>& traceInfo, const World* pWorld, int depth) override;
};