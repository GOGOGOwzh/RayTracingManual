#include "MeshTriangle.h"
#include "CSObject.h"
#include "OBJ_Loader.h"

MeshTriangle::MeshTriangle(const Vec3* verts, const unsigned int* vertsIndex, const Vec2* uvIndex, const unsigned int triangleCount, const Material& mat)
{
	m_Mat = mat;
	m_iTriangleCount = triangleCount;
	auto trianglePoint = m_iTriangleCount * 3;

	for (unsigned i = 0; i < trianglePoint; i++)
	{
		if (vertsIndex[i] > m_iVertsCount)
		{
			m_iVertsCount = vertsIndex[i];
		}
	}

	m_iVertsCount += 1;

	m_pVerts = new Vec3[m_iVertsCount];
	memcpy(m_pVerts, verts, m_iVertsCount * sizeof(Vec3));

	m_pVertsIndex = new unsigned int[trianglePoint];
	memcpy(m_pVertsIndex, vertsIndex, trianglePoint * sizeof(unsigned int));

	m_pVertsUVIndex = new Vec2[m_iVertsCount];
	memcpy(m_pVertsUVIndex, uvIndex, m_iVertsCount * sizeof(Vec2));

	m_iTotalVertsCount = trianglePoint;

	ConstructTriangles();
}

MeshTriangle::MeshTriangle(const std::string& fileName, const Vec3& pos, const Material& mat, const float scale)
{
	m_Mat = mat;
	m_scale = scale;
	m_pos = pos;
	m_strObjName = fileName;
	ConstructTriangles(m_strObjName);
}

void MeshTriangle::ConstructTriangles()
{
	auto min_pos = Vec3(std::numeric_limits<float>::infinity());
	auto max_pos = Vec3(-std::numeric_limits<float>::infinity());

	for (unsigned i = 0; i < m_iTriangleCount; i++)
	{
		Triangle triangle;
		for (unsigned j = 0; j < 3; j++)
		{
			unsigned index = i * 3 + j;
			unsigned vertsIndex = m_pVertsIndex[index];
			triangle.Point[j] = m_pVerts[vertsIndex];
			triangle.UVIndex[j] = m_pVertsUVIndex[vertsIndex];

			min_pos = Min(min_pos, triangle.Point[j]);
			max_pos = Max(max_pos, triangle.Point[j]);
		}
		triangle.Mat = m_Mat;
		CalcTriangleNormalAndArea(triangle);
		BuildBoundingBox(triangle);
		m_vecTriangles.push_back(triangle);
	}
}

void MeshTriangle::ConstructTriangles(const std::string& fileName)
{
	OBJL::Loader objLoader;
	if (!objLoader.LoadFile(fileName))
	{
		m_vecTriangles.clear();
		return;
	}

	auto min_pos = Vec3(std::numeric_limits<float>::infinity());
	auto max_pos = Vec3(-std::numeric_limits<float>::infinity());

	for (int meshIndex = 0; meshIndex < objLoader.LoadedMeshes.size(); meshIndex++)
	{
		//auto& mesh = objLoader.LoadedMeshes[meshIndex];
		//auto mat = MakeMatByOBJ(mesh.MeshMaterial);
		//m_objMatVec.push_back(mat);

		///这个地方可以创建纹理
		for (int i = 0; i < objLoader.LoadedIndices.size(); i += 3)
		{
			Triangle triangle;
			for (int j = 0; j < 3; j++)
			{
				auto vertIndex = objLoader.LoadedIndices[i + j];
				auto& loaderVert = objLoader.LoadedVertices[vertIndex];
				triangle.Point[j] = Vec3(loaderVert.Position.X, loaderVert.Position.Y, loaderVert.Position.Z);
				triangle.PointNoraml[j] = Vec3(loaderVert.Normal.X, loaderVert.Normal.Y, loaderVert.Normal.Z);
				triangle.UVIndex[j] = Vec2(loaderVert.TextureCoordinate.X, loaderVert.TextureCoordinate.Y);

				triangle.Point[j] *= m_scale;
				triangle.Point[j] += m_pos;

				min_pos = Min(min_pos, triangle.Point[j]);
				max_pos = Max(max_pos, triangle.Point[j]);
			}

			triangle.Mat = m_Mat;

			m_iTriangleCount++;
			CalcTriangleNormalAndArea(triangle);
			BuildBoundingBox(triangle);
			m_vecTriangles.push_back(triangle);
		}
	}
}


