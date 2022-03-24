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
	MAT_MC_DIFFUSE,
	MAT_MC_COK_TORRANCE,
};

class World;
class Material : public Object
{
public:
	char m_matType = MAT_NONE;
	Color3 m_albedo = Color3(1);
	Color3 m_emission;

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

	void SetEmission(const Color3& col) {
		m_emission = col;
	}

	Color3 GetEmission() {
		return m_emission;
	}

	bool HasEmission() {
		return !m_emission.IsZero();
	}

	virtual Color3 Shadring(const std::optional<TraceInfo>& traceInfo, const World* pWorld, int depth)
	{
		return m_albedo;
	};

	virtual Color3 BRDF(const Vector3f & wi,const Vector3f & wo,const Vector3f & N) {
		return m_albedo;
	}
	virtual float PDF(const Vector3f& wi, const Vector3f& wo, const Vector3f& N) {
		return 0;
	}
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

class MatMCDiffuse : public Material
{
public:
	MatMCDiffuse() :Material(MAT_MC_DIFFUSE) {}
	MatMCDiffuse(const Color3& albedo, const Color3& emssion) :MatMCDiffuse()
	{
		m_albedo = albedo;
		m_emission = emssion;
	}

	virtual Vector3f BRDF(const Vector3f& wi, const Vector3f& wo, const Vector3f& N) override;
	virtual float PDF(const Vector3f& wi, const Vector3f& wo, const Vector3f& N)override;
};

class MatCookTorrance : public Material
{
	float m_F0 = 0.04;
	float m_Roughness = 0.4;
	Color3 m_ks;
public:
	MatCookTorrance():Material(MAT_MC_COK_TORRANCE){}
	MatCookTorrance(const Color3& albedo, const Color3& kSpcular,const float f0 = 0.04,const float roughness = 0.35) :MatCookTorrance()
	{
		m_albedo = albedo;
		m_ks = kSpcular;
		m_F0 = f0;
		m_Roughness = roughness;
	}
	
	float FresnelTerm(const Vector3f& l, const Vector3f& v);
	float GeometrySchlickGGX(const Vector3f &l,const Vector3f &v, const Vector3f &n);
	float DistributionGGX(const Vector3f& l, const Vector3f& v, const Vector3f& n);
	virtual Vector3f BRDF(const Vector3f& wi, const Vector3f& wo, const Vector3f& N) override;
	virtual float PDF(const Vector3f& wi, const Vector3f& wo, const Vector3f& N)override;
};