#include "World.h"
#include "MeshTriangle.h"
#include "BVH.h"

void World::Init()
{
	CreateWorld();
}

void World::Tick(DWORD deltaTime)
{

}

void World::Render()
{

}

int World::GetSphereByteWidth()
{
	return sizeof(Sphere) * m_VecSphere.size();
}

int World::GetSphereStride()
{
	return sizeof(Sphere);
}

std::vector<Sphere>& World::GetSphereVec()
{
	return m_VecSphere;
}

int World::GetTriangleByteWidth()
{
	return m_VecTriangles.size() * sizeof(Triangle);
}

int World::GetTriangleStride()
{
	return sizeof(Triangle);
}

std::vector<Triangle>& World::GetTriangleVec()
{
	return m_VecTriangles;
}

int World::GetLightTriangleByteWidth()
{
	return m_VecLightTriangles.size() * sizeof(Triangle);
}

std::vector<Triangle>& World::GetLightTriangleVec()
{
	return m_VecLightTriangles;
}

int World::GetBVHByteWidth()
{
	return m_BVH->GetCSBVHNodeWidth();
}

int World::GetBVHStride()
{
	return m_BVH->GetCSBVHNodeStride();
}

std::vector<CSBVH_Node>& World::GetBVHVec()
{
	return m_BVH->GetCSBVHNode();
}

void World::CreateWorld()
{
	CreateSphere();
	CreateMeshTriangle();
	BuildBVH();
}

void World::CreateSphere()
{
	Material matRed;
	matRed.Type = MAT_LAMBERT;
	matRed.Albedo = Vec3(1, 0, 0);

	Material matGreen;
	matGreen.Type = MAT_LAMBERT;
	matGreen.Albedo = Vec3(0, 1, 0);

	Sphere sphereRed;
	sphereRed.Center = Vec3(-1.5f, 0, 0);
	sphereRed.Mat = matRed;

	Sphere sphereGreen;
	sphereGreen.Center = Vec3(1.5f, 0, 0);
	sphereGreen.Mat = matGreen;

	m_VecSphere.push_back(sphereRed);
	m_VecSphere.push_back(sphereGreen);
}

void World::CreateMeshTriangle()
{
	Material matRed;
	matRed.Type = MAT_MC_DIFFUSE;
	matRed.Albedo = Vec3(0.63f, 0.065f, 0.05f);
	matRed.Emission = Vec3(0, 0, 0);

	Material matGreen;
	matGreen.Type = MAT_MC_DIFFUSE;
	matGreen.Albedo = Vec3(0.14f, 0.45f, 0.091f);
	matGreen.Emission = Vec3(0, 0, 0);

	Material matWhite;
	matWhite.Type = MAT_MC_DIFFUSE;
	matWhite.Albedo = Vec3(0.725f, 0.71f, 0.68f);
	matWhite.Emission = Vec3(0, 0, 0);

	Vec3 lightEm = 8.0f * Vec3(0.747f + 0.058f, 0.747f + 0.258f, 0.747f) + 15.6f * Vec3(0.740f + 0.287f, 0.740f + 0.160f, 0.740f) + 18.4f * Vec3(0.737f + 0.642f, 0.737f + 0.159f, 0.737f);
	Material matLight;
	matLight.Type = MAT_MC_DIFFUSE;
	matLight.Albedo = Vec3(0.65f, 0.65f, 0.65f);
	matLight.Emission = lightEm;

	int trianglesCount = 0;
	
	MeshTriangle floor("floor.obj", Vec3(0,0,0), matWhite);
	trianglesCount += floor.GetTrianglesCount();
	
	MeshTriangle shortbox("shortbox.obj", Vec3(0, 0, 0), matWhite);
	trianglesCount += shortbox.GetTrianglesCount();
	
	MeshTriangle tallbox("tallbox.obj", Vec3(0, 0, 0), matWhite);
	trianglesCount += tallbox.GetTrianglesCount();
	
	MeshTriangle left("left.obj", Vec3(0, 0, 0), matRed);
	trianglesCount += left.GetTrianglesCount();

	MeshTriangle right("right.obj", Vec3(0, 0, 0), matGreen);
	trianglesCount += right.GetTrianglesCount();
	
	MeshTriangle light("light.obj", Vec3(0, 0, 0), matLight);
	trianglesCount += light.GetTrianglesCount();

	m_VecTriangles.reserve(trianglesCount);

	AddTriangleToVec(&floor);
	AddTriangleToVec(&shortbox);
	AddTriangleToVec(&tallbox);
	AddTriangleToVec(&left);
	AddTriangleToVec(&right);
	AddTriangleToVec(&light);
}

void World::AddTriangleToVec(MeshTriangle* pMesh)
{
	auto vecTriangle = pMesh->GetTriangleVec();
	for (auto & triangle:vecTriangle)
	{
		triangle.Index = m_VecTriangles.size();
		for (int i=0;i<3;i++)
		{
			printf("Triangle:%d X_%d:%f Y_%d:%f Z_%d:%f\n", triangle.Index, i,triangle.Point[i].x, i,triangle.Point[i].y, i,triangle.Point[i].z);
		}
		
		m_VecTriangles.push_back(triangle);

		if (triangle.Mat.Emission != Vec3::Zero)
		{
			m_VecLightTriangles.push_back(triangle);
			printf("Light Triangle Index:%d \n", triangle.Index);
		}
	}
}

void World::BuildBVH()
{
	m_BVH = std::make_shared<BVH>(m_VecTriangles);
}
