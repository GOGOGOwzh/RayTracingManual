#include "World.h"
#include "TraceInfo.h"
#include "Sphere.h"
#include "Material.h"
#include "Light.h"
#include "Global.h"
#include "MeshTriangle.h"
#include "BVH.h"

const float P_RR = 0.8f;

World::World()
{

}

World::~World()
{

}

void World::CreateOnlyDiffuseWorldObj()
{
	auto red = std::make_shared<MatMCDiffuse>(Color3(0.63f, 0.065f, 0.05f), Color3(0.0f));
	auto green = std::make_shared<MatMCDiffuse>(Color3(0.14f, 0.45f, 0.091f), Color3(0.0f));
	auto white = std::make_shared<MatMCDiffuse>(Color3(0.725f, 0.71f, 0.68f), Color3(0.0f));
	auto cook_torrance = std::make_shared<MatCookTorrance>(Color3(0, 0, 1), Color3(0.0f));

	Color3 lightEm = 8.0f * Color3(0.747f + 0.058f, 0.747f + 0.258f, 0.747f) + 15.6f * Color3(0.740f + 0.287f, 0.740f + 0.160f, 0.740f) + 18.4f * Color3(0.737f + 0.642f, 0.737f + 0.159f, 0.737f);
	auto light = std::make_shared<MatMCDiffuse>(Color3(0.65f), lightEm);

	auto floor = std::make_shared<MeshTriangle>("floor.obj",Point3(0));
	floor->PushMaterial(white);

	auto shortbox = std::make_shared<MeshTriangle>("shortbox.obj", Point3(0));
	shortbox->PushMaterial(white);

	auto tallbox = std::make_shared<MeshTriangle>("tallbox.obj", Point3(0));
	tallbox->PushMaterial(white);

	auto left = std::make_shared<MeshTriangle>("left.obj", Point3(0));
	left->PushMaterial(red);

	auto right = std::make_shared<MeshTriangle>("right.obj", Point3(0));
	right->PushMaterial(green);
	
	auto lightObj = std::make_shared<MeshTriangle>("light.obj", Point3(0));
	lightObj->PushMaterial(light);

	m_vecWorldObject.push_back(floor);
	m_vecWorldObject.push_back(shortbox);
	m_vecWorldObject.push_back(tallbox);
	m_vecWorldObject.push_back(left);
	m_vecWorldObject.push_back(right);
	m_vecWorldObject.push_back(lightObj);
	if (m_bUseBVH)
	{
		BuildBVH();
	}
}

void World::CreateCookTorranceRelectWorldObj()
{
	auto red = std::make_shared<MatMCDiffuse>(Color3(0.63f, 0.065f, 0.05f), Color3(0.0f));
	auto green = std::make_shared<MatMCDiffuse>(Color3(0.14f, 0.45f, 0.091f), Color3(0.0f));
	auto white = std::make_shared<MatMCDiffuse>(Color3(0.725f, 0.71f, 0.68f), Color3(0.0f));
	auto cook_torrance = std::make_shared<MatCookTorrance>(Color3(0), Color3(1),0.0f,0.4);

	Color3 lightEm = 8.0f * Color3(0.747f + 0.058f, 0.747f + 0.258f, 0.747f) + 15.6f * Color3(0.740f + 0.287f, 0.740f + 0.160f, 0.740f) + 18.4f * Color3(0.737f + 0.642f, 0.737f + 0.159f, 0.737f);
	auto light = std::make_shared<MatMCDiffuse>(Color3(0.65f), lightEm);

	auto floor = std::make_shared<MeshTriangle>("floor.obj", Point3(0));
	floor->PushMaterial(white);

	auto left = std::make_shared<MeshTriangle>("left.obj", Point3(0));
	left->PushMaterial(red);

	auto right = std::make_shared<MeshTriangle>("right.obj", Point3(0));
	right->PushMaterial(green);

	auto lightObj = std::make_shared<MeshTriangle>("light.obj", Point3(0));
	lightObj->PushMaterial(light);

	auto sphere = std::make_shared<Sphere>(Vector3f(150, 100, 300), 100);
	sphere->PushMaterial(cook_torrance);

	m_vecWorldObject.push_back(floor);
	m_vecWorldObject.push_back(left);
	m_vecWorldObject.push_back(right);
	m_vecWorldObject.push_back(lightObj);
	m_vecWorldObject.push_back(sphere);

	if (m_bUseBVH)
	{
		BuildBVH();
	}
}

void World::CreateCookTorranceRelectRefractWorldObj()
{
	auto red = std::make_shared<MatMCDiffuse>(Color3(0.63f, 0.065f, 0.05f), Color3(0.0f));
	auto green = std::make_shared<MatMCDiffuse>(Color3(0.14f, 0.45f, 0.091f), Color3(0.0f));
	auto white = std::make_shared<MatMCDiffuse>(Color3(0.725f, 0.71f, 0.68f), Color3(0.0f));
	auto refract = std::make_shared<MatReflectRefract>(Color3(1, 1, 1), 1.5);

	Color3 lightEm = 8.0f * Color3(0.747f + 0.058f, 0.747f + 0.258f, 0.747f) + 15.6f * Color3(0.740f + 0.287f, 0.740f + 0.160f, 0.740f) + 18.4f * Color3(0.737f + 0.642f, 0.737f + 0.159f, 0.737f);
	auto light = std::make_shared<MatMCDiffuse>(Color3(0.65f), lightEm);

	auto floor = std::make_shared<MeshTriangle>("floor.obj", Point3(0));
	floor->PushMaterial(white);

	auto left = std::make_shared<MeshTriangle>("left.obj", Point3(0));
	left->PushMaterial(red);

	auto right = std::make_shared<MeshTriangle>("right.obj", Point3(0));
	right->PushMaterial(green);

	auto lightObj = std::make_shared<MeshTriangle>("light.obj", Point3(0));
	lightObj->PushMaterial(light);

	auto sphere = std::make_shared<Sphere>(Vector3f(150, 100, 300), 100);
	sphere->PushMaterial(refract);

	m_vecWorldObject.push_back(floor);
	m_vecWorldObject.push_back(left);
	m_vecWorldObject.push_back(right);
	m_vecWorldObject.push_back(lightObj);
	m_vecWorldObject.push_back(sphere);

	if (m_bUseBVH)
	{
		BuildBVH();
	}
}

void World::BuildBVH()
{
	std::vector<Object*> vecObj;
	for (auto obj: m_vecWorldObject)
	{
		vecObj.push_back(obj.get());
	}
	m_pBVH = std::make_shared<BVH>(vecObj);
}

std::optional<TraceInfo> World::RayTrace(const Ray& ray) const
{
	if (m_bUseBVH)
	{
		return RayTraceWithBVH(ray);
	}

	return RayTrace(ray.GetOrgPos(), ray.GetDir());
}

std::optional<TraceInfo> World::RayTrace(const Point3& pos, const Vector3f& dir) const
{
	std::optional<TraceInfo> traceInfo;
	traceInfo.emplace();

	float fraction = std::numeric_limits<float>::max();
	bool bTrace(false);
	
	std::shared_ptr<Object> traceObj;
	for (auto & obj:m_vecWorldObject)
	{
		if (obj->RayTrace(pos, dir.Nor(), traceInfo) && traceInfo->Fraction < fraction)
		{
			fraction = traceInfo->Fraction;
			bTrace = true;
			traceObj = obj;
		}
	}

	if (bTrace && traceObj)
	{
		traceInfo->Lights = m_vecLights;
		traceInfo->HitObj = traceObj.get();
		traceInfo->HitObjMat = traceObj->GetObjectMaterial();

		traceInfo->HitRay = std::make_shared<Ray>(pos, dir.Nor(), fraction);
		traceInfo->HitPoint = traceInfo->HitRay->AtTime();

		SurfaceProperty proerty;
		if (traceObj->GetSurfaceProperty(traceInfo, proerty))
		{
			traceInfo->HitNormal = proerty.normal;
			traceInfo->UV = proerty.uv;
			traceInfo->CheckFrontFace();
		}
	}

	return traceInfo;
}

std::optional<TraceInfo> World::RayTraceWithBVH(const Ray& ray) const
{
	std::optional<TraceInfo> traceInfo;
	traceInfo.emplace();

	if (m_pBVH && m_pBVH->RayTrace(ray.GetOrgPos(),ray.GetDir(), traceInfo) && traceInfo->HitObj)
	{
		traceInfo->Lights = m_vecLights;
		traceInfo->HitObjMat = traceInfo->HitObj->GetObjectMaterial();

		traceInfo->HitRay = std::make_shared<Ray>(ray.GetOrgPos(), ray.GetDir().Nor(), traceInfo->Fraction);
		traceInfo->HitPoint = traceInfo->HitRay->AtTime();

		SurfaceProperty proerty;
		if (traceInfo->HitObj->GetSurfaceProperty(traceInfo, proerty))
		{
			traceInfo->HitNormal = proerty.normal;
			traceInfo->UV = proerty.uv;
			traceInfo->CheckFrontFace();
		}
	}

	return traceInfo;
}

Vector3f random_in_unit_sphere() {
	while (true) {
		auto p = Vector3f(Get_random_float(-1,1), Get_random_float(-1, 1),Get_random_float(-1, 1));
		if (p.LenSquared() >= 1) continue;
		return p;
	}
}

Color3 World::RayTraceColor(const Ray& ray, int depth) const
{
	auto traceInfo = RayTrace(ray);
	if (!traceInfo->HaveTrace() || depth >= m_iSceneMaxDepth)
	{
		return Color3(0.0, 0.0, 0.0);
	}
	
	if (traceInfo->HitObjMat && traceInfo->HitObjMat->HasEmission())
	{
		return traceInfo->HitObjMat->GetEmission();
	}

	if (traceInfo->HitObjMat->GetMatType() == MAT_REFLECT_REFRACT)
	{
		return traceInfo->HitObjMat->Shadring(traceInfo, this, depth + 1);
	}

	Color3 lightColorDir;
	Color3 lightColorInDir;

	auto hitPos = traceInfo->HitPoint;
	auto hitNor = traceInfo->HitNormal;
	auto hitMat = traceInfo->HitObjMat;
	
	Vector3f sampleAreaLightPos;
	Vector3f sampleAreaLightNor;
	float sampleAreaLightPDF = 0.00001f;

	SampleAreaLight(sampleAreaLightPos, sampleAreaLightNor, sampleAreaLightPDF);
	
	Vector3f wi = Normalize(sampleAreaLightPos - hitPos);
	Vector3f wo = (-ray.GetDir()).Nor();

	auto testLightTrace = RayTrace(Ray(hitPos, wi));
	if (testLightTrace->HaveTrace() && testLightTrace->HitObjMat->HasEmission()&& hitMat)
	{
		auto len = (sampleAreaLightPos - hitPos).Len();
		auto distance2 = len * len;
		auto lightMat = testLightTrace->HitObjMat;
		lightColorDir = lightMat->GetEmission() *
			hitMat->BRDF(wi, wo, hitNor) *
			DotProduct(wi, hitNor) *
			DotProduct(-wi, sampleAreaLightNor.Nor()) /
			distance2 /
			sampleAreaLightPDF;
	}

	float prr = Get_random_float();
	if (prr <= P_RR && hitMat)
	{
		wi = HemisphereUniformSample(wo, hitNor).Nor();
		auto testObjTrace = RayTrace(Ray(hitPos, wi));
		if (testObjTrace->HaveTrace() && !testObjTrace->HitObjMat->HasEmission())
		{
			lightColorInDir = RayTraceColor(Ray(hitPos, wi), 0) *
				hitMat->BRDF(wi, wo, hitNor) *
				DotProduct(wi, hitNor) /
				hitMat->PDF(wi, wo, hitNor) /
				P_RR;
		}
	}

	return lightColorDir + lightColorInDir;
}

void World::SampleAreaLight(Vector3f& pos, Vector3f& normal, float& pdf) const
{
	int lightCount = 0;
	for (auto obj:m_vecWorldObject)
	{
		auto mat = obj->GetObjectMaterial();
		if (mat && mat->HasEmission())
		{
			lightCount++;
		}
	}

	if (lightCount > 1)
	{
		lightCount = int(Get_random_float(1, lightCount));
	}

	int randomCount = 0;
	for (auto obj : m_vecWorldObject)
	{
		auto mat = obj->GetObjectMaterial();
		if (mat && mat->HasEmission())
		{
			randomCount++;
			if (randomCount >= lightCount)
			{
				obj->Sample(pos, normal, pdf);
				//pdf = 1.0f / obj->GetArea();
				break;
			}
		}
	}

}

void World::SampleLight(Vector3f& pos, Vector3f& normal, float& pdf) const
{
	float emit_area_sum = 0;
	for (uint32_t k = 0; k < m_vecWorldObject.size(); ++k) {
		if (m_vecWorldObject[k]->HasEmit()) {
			emit_area_sum += m_vecWorldObject[k]->GetArea();
		}
	}
	float p = Get_random_float() * emit_area_sum;
	emit_area_sum = 0;
	for (uint32_t k = 0; k < m_vecWorldObject.size(); ++k) {
		if (m_vecWorldObject[k]->HasEmit()) {
			emit_area_sum += m_vecWorldObject[k]->GetArea();
			if (p <= emit_area_sum) {
				m_vecWorldObject[k]->Sample(pos,normal,pdf);
				break;
			}
		}
	}
}
