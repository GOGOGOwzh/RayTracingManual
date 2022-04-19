#include "World.h"
#include "MeshTriangle.h"

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

void World::CreateWorld()
{
	CreateSphere();
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
	matRed.Type = MAT_MC_DIFFUSE;
	matRed.Albedo = Vec3(0.14f, 0.45f, 0.091f);
	matRed.Emission = Vec3(0, 0, 0);

	Material matWhite;
	matRed.Type = MAT_MC_DIFFUSE;
	matRed.Albedo = Vec3(0.725f, 0.71f, 0.68f);
	matRed.Emission = Vec3(0, 0, 0);

	Vec3 lightEm = 8.0f * Vec3(0.747f + 0.058f, 0.747f + 0.258f, 0.747f) + 15.6f * Vec3(0.740f + 0.287f, 0.740f + 0.160f, 0.740f) + 18.4f * Vec3(0.737f + 0.642f, 0.737f + 0.159f, 0.737f);
	Material matLight;
	matRed.Type = MAT_MC_DIFFUSE;
	matRed.Albedo = Vec3(0.65f, 0.65f, 0.65f);
	matRed.Emission = lightEm;

	MeshTriangle floor("floor.obj", Vec3(0,0,0), matWhite);
	AddTriangleToVec(&floor);

	MeshTriangle shortbox("shortbox.obj", Vec3(0, 0, 0), matWhite);
	AddTriangleToVec(&shortbox);

	MeshTriangle tallbox("tallbox.obj", Vec3(0, 0, 0), matWhite);
	AddTriangleToVec(&tallbox);

	MeshTriangle left("left.obj", Vec3(0, 0, 0), matRed);
	AddTriangleToVec(&left);

	MeshTriangle right("right.obj", Vec3(0, 0, 0), matGreen);
	AddTriangleToVec(&right);

	MeshTriangle light("light.obj", Vec3(0, 0, 0), matLight);
	AddTriangleToVec(&light);


}

void World::AddTriangleToVec(MeshTriangle* pMesh)
{
	auto vecTriangle = pMesh->GetTriangleVec();
	for (auto & triangle:vecTriangle)
	{
		triangle.Index = m_VecTriangles.size();
		m_VecTriangles.push_back(triangle);
	}
}
