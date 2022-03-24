#include "Material.h"
#include "TraceInfo.h"
#include "Light.h"
#include "World.h"
#include "Global.h"
#include "MeshTriangle.h"
#include "Triangle.h"

/// <summary>
/// MatLambert
/// </summary>
/// <param name="traceInfo"></param>
/// <param name="pWorld"></param>
/// <returns></returns>
Color3 MatLambert::Shadring(const std::optional<TraceInfo>& traceInfo, const World* pWorld, int depth)
{
	Vector3f specular, diffuse;

	auto hitPoint = traceInfo->HitPoint;
	auto hitNormal = traceInfo->HitNormal;
	hitNormal.Normalize();
	auto viewDir = traceInfo->HitRay->GetDir();

	//这句代码抄自Games101 发现这样做噪点会少
	Vector3f shadowPointOrig = (DotProduct(viewDir, hitNormal) < 0) ?
		hitPoint + hitNormal * 0.00001 :
		hitPoint - hitNormal * 0.00001;

	for (auto & light:traceInfo->Lights)
	{	
		auto lightDir = light->GetPos() - hitPoint;
		auto lightDistance = DotProduct(lightDir, lightDir);
		lightDir.Normalize();

		auto ldotn =std::max(0.0f,DotProduct(lightDir, hitNormal));
		auto shadowInfo = pWorld->RayTrace(Ray(shadowPointOrig, lightDir));

		bool isInShadow = shadowInfo->HaveTrace() && shadowInfo->Fraction * shadowInfo->Fraction < lightDistance;
		diffuse += isInShadow ? 0 : light->GetIntensity() * ldotn;

		auto reflection = Reflect(-lightDir, hitNormal).Nor();
		specular += std::powf(std::max(0.0f,-DotProduct(reflection,viewDir)),m_specularExponent) * light->GetIntensity();
		
	}
	Color3 triangleDiffuse = Color3(1);

	if (traceInfo->HitObj->GetObjType() == TYPE_MESH_TRIANGLE)
	{
		auto mesh = dynamic_cast<MeshTriangle*>(traceInfo->HitObj);
		triangleDiffuse = mesh->GetDiffuseColor(traceInfo->UV);
	}
	else if (traceInfo->HitObj->GetObjType() == TYPE_TRIANGLE)
	{
		auto triangle = dynamic_cast<Triangle*>(traceInfo->HitObj);
		triangleDiffuse = triangle->PMesh->GetDiffuseColor(traceInfo->UV);
	}
	auto finalColor = diffuse * m_albedo * triangleDiffuse* 0.6 + specular * 0;
	return finalColor;
}

/// <summary>
/// MatReflect
/// </summary>
/// <param name="traceInfo"></param>
/// <param name="pWorld"></param>
/// <returns></returns>
Color3 MatReflect::Shadring(const std::optional<TraceInfo>& traceInfo, const World* pWorld, int depth)
{
	auto hitPoint = traceInfo->HitPoint;
	auto hitNormal = traceInfo->HitNormal;
	auto viewDir = traceInfo->HitRay->GetDir();
	viewDir.Normalize();

	auto reflection = Reflect(viewDir, hitNormal).Nor();

	//这句代码抄自Games101 发现这样做噪点会少
	Vector3f reflectionRayOrig = (DotProduct(reflection, hitNormal) < 0) ?
		hitPoint + hitNormal * 0.00001 :
		hitPoint - hitNormal * 0.00001;

	auto reflectionColor = pWorld->RayTraceColor(Ray(reflectionRayOrig, reflection), depth + 1);

	auto finalColor = reflectionColor * m_albedo;
	return finalColor;
}


/// <summary>
/// MatReflectRefract
/// </summary>
/// <param name="traceInfo"></param>
/// <param name="pWorld"></param>
/// <param name="depth"></param>
/// <returns></returns>
Color3 MatReflectRefract::Shadring(const std::optional<TraceInfo>& traceInfo, const World* pWorld, int depth)
{
	auto hitPoint = traceInfo->HitPoint;
	auto hitNormal = traceInfo->HitNormal;
	auto viewDir = traceInfo->HitRay->GetDir();
	viewDir.Normalize();

	auto reflection = Reflect(viewDir, hitNormal).Nor();
	auto refraction = Refract(viewDir, hitNormal, m_refractivity).Nor();

	//这句代码抄自Games101 发现这样做噪点会少
	Vector3f reflectionRayOrig = (DotProduct(reflection, hitNormal) < 0) ?
		hitPoint - hitNormal * 0.00001 :
		hitPoint + hitNormal * 0.00001;
		
	auto dotRe = DotProduct(refraction, hitNormal);
	Vector3f refractionRayOrig = (dotRe < 0) ?
		hitPoint - hitNormal * 0.00001 :
		hitPoint + hitNormal * 0.00001;

	auto reflectionColor = pWorld->RayTraceColor(Ray(reflectionRayOrig, reflection), depth + 1);
	auto refractionColor = pWorld->RayTraceColor(Ray(refractionRayOrig, refraction), depth + 1);

	auto fresnel = Fresnel(viewDir, hitNormal, m_refractivity);

	auto finalColor = m_albedo * reflectionColor * fresnel + refractionColor * m_albedo * (1 - fresnel);

	return finalColor;
}

/// <summary>
/// MatMCDiffuse
/// </summary>

Vector3f MatMCDiffuse::BRDF(const Vector3f& wi, const Vector3f& wo, const Vector3f& N)
{
	float cosine = DotProduct(wo, N);
	if (cosine > 0)
	{
		return m_albedo / M_PI;
	}
	else {
		return Vector3f(0.0);
	}
}

float MatMCDiffuse::PDF(const Vector3f& wi, const Vector3f& wo, const Vector3f& N)
{
	float cosine = DotProduct(wo, N);
	if (cosine > 0)
	{
		return 1.0 / (2.0 * M_PI);
	}
	else {
		return 0.0;
	}
}

/// <summary>
/// MatCookTorrance
/// </summary>

float MatCookTorrance::FresnelTerm(const Vector3f& l, const Vector3f& v)
{
	Vector3f halfV = Normalize(l + v);
	return m_F0 + (1 - m_F0) * std::pow((1 - DotProduct(v, halfV)), 5);
}

float MatCookTorrance::GeometrySchlickGGX(const Vector3f& l, const Vector3f& v, const Vector3f& n)
{
	float r = m_Roughness + 1;
	float k = r * r / 8.0f;
	float _k = 1 - k;
	float ndotv = std::max(DotProduct(n, v), 0.0f);
	float g1 = ndotv / (ndotv * (_k) + k);

	float ndotl = std::max(DotProduct(n, l), 0.0f);
	float g2 = ndotl / (ndotl * (_k) + k);

	return g1 * g2;
}

float MatCookTorrance::DistributionGGX(const Vector3f& l, const Vector3f& v, const Vector3f& n)
{
	Vector3f halfV = Normalize(l + v);
	float a = m_Roughness * m_Roughness;
	float a2 = a * a;
	float ndoth = std::max(DotProduct(n, halfV),0.0f);
	float ndoth2 = ndoth * ndoth;
	float a2_1 = a2 - 1;
	float denominator = ndoth2 * a2_1 + 1.0f;
	denominator = M_PI * denominator * denominator;
	return a2 / denominator;
}

Vector3f MatCookTorrance::BRDF(const Vector3f& wi, const Vector3f& wo, const Vector3f& N)
{
	//Cook-Torrance BRDF
	//fr = kd * diffuse + ks * f_cook_torrance
	//f_cook_torrance = F(v,h)G(l,v)D(h)/4(n·l)(n·v) l:入射光 v:反射光 n:表面法线 h:半程向量(l+v)/|l+v|
	float cosine = DotProduct(wo, N);
	if (cosine > 0)
	{
		Vector3f l = wi;
		Vector3f v = wo;
		float F = Fresnel(wi,N,1.85);
		float G = GeometrySchlickGGX(l, v, N);
		float D = DistributionGGX(l, v, N);
		float numerator = F * G * D;
		float denominator = std::max((4 * DotProduct(N, l) * DotProduct(N, v)), 0.001f);
		float result = numerator / denominator;

		Vector3f specular = m_ks * result;
		Vector3f diffuse =  m_albedo / M_PI;
		return (1-F)*diffuse + specular;
	}
	else {
		return Vector3f(0.0);
	}
}

float MatCookTorrance::PDF(const Vector3f& wi, const Vector3f& wo, const Vector3f& N)
{
	float cosine = DotProduct(wo, N);
	if (cosine > 0)
	{
		return 1.0 / (2.0 * M_PI);
	}
	else {
		return 0.0;
	}
}
