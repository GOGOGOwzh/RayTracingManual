#include "World.h"
#include "TraceInfo.h"
#include "Sphere.h"
#include "Material.h"
#include "Light.h"
#include "Global.h"
#include "MeshTriangle.h"
#include "BVH.h"

World::World()
{

}

World::~World()
{

}

void World::CreateWorldObj()
{
	auto light = std::make_shared<Light>(Point3(100, 100, 100),1);
	auto light1 = std::make_shared<Light>(Point3(0, 0, 100), 1);

	auto backLambert = std::make_shared<MatLambert>(Color3(1, 1, 1));
	auto matLambert1 = std::make_shared<MatLambert>(Color3(0, 1, 0.5));
	auto matReflect = std::make_shared<MatReflect>(Color3(1, 0.92, 0.54));
	auto matReflectAndRefract = std::make_shared<MatReflectRefract>(Color3(1, 1, 1),1.5);

	auto testObj = std::make_shared<MeshTriangle>("person.obj",Point3(7,0,-7),0.1);
	auto bunnyObj = std::make_shared<MeshTriangle>("bunny.obj", Point3(-2, 2, -20), 35);
	
	Vector3f verts[4] = { {-5,-3,-6}, {5,-3,-6}, {5,-3,-16}, {-5,-3,-16} };
	unsigned int vertIndex[6] = { 0, 1, 3, 1, 2, 3 };
	Vector2f uv[4] = { {0, 0}, {1, 0}, {1, 1}, {0, 1} };
	auto background = std::make_shared<MeshTriangle>(verts, vertIndex, uv, 2);
	background->PushMaterial(backLambert);

	auto obj1 = std::make_shared<Sphere>(Point3(0.3, -0.5, -8), 1.5);
	obj1->PushMaterial(matReflectAndRefract);

	auto obj2 = std::make_shared<Sphere>(Point3(3, -0.5, -8), 1);
	obj2->PushMaterial(matReflect);

	auto obj3 = std::make_shared<Sphere>(Point3(-1.2, 0, -12), 2);
	obj3->PushMaterial(matLambert1);

	m_vecLights.push_back(light);
	m_vecLights.push_back(light1);
	m_vecWorldObject.push_back(obj1);
	m_vecWorldObject.push_back(background);
	m_vecWorldObject.push_back(obj2);
	m_vecWorldObject.push_back(obj3);
	m_vecWorldObject.push_back(testObj);
	m_vecWorldObject.push_back(bunnyObj);
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
	if (depth >= m_iSceneMaxDepth)
	{
		return Color3(0.0, 0.0, 0.0);
	}

	auto traceInfo = RayTrace(ray);
	if (traceInfo->HaveTrace())
	{
		//Point3 target = traceInfo->hitPoint + traceInfo->normal + random_in_unit_sphere();
		//return 0.5 * RayTraceColor(Ray(traceInfo->hitPoint, target - traceInfo->hitPoint), depth + 1);
		return traceInfo->HitObjMat->Shadring(traceInfo, this,depth+1);
	}
	
	
	auto norDirection = ray.GetDir().Nor();
	auto t = 0.5 * (norDirection.y + 1.0);
	return (1.0 - t) * Color3(1.0, 1.0, 1.0) + t * Color3(0.5, 0.7, 1.0);
}
