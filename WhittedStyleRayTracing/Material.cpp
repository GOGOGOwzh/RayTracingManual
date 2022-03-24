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
