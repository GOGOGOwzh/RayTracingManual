#include "World.h"

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
