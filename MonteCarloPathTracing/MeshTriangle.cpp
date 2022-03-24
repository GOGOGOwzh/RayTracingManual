#include "MeshTriangle.h"
#include "TraceInfo.h"
#include "OBJ_Loader.h"
#include "Material.h"
#include "Global.h"
#include "BoundingBox.h"
#include "BVH.h"
#include "Triangle.h"

#include <array>

/// <summary>
/// MeshTriangle
/// </summary>
/// 

MeshTriangle::MeshTriangle(const Vector3f* verts, const unsigned int* vertsIndex, const Vector2f* uvIndex, const unsigned int triangleCount) :MeshTriangle()
{
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

	m_pVerts = new Vector3f[m_iVertsCount];
	memcpy(m_pVerts, verts, m_iVertsCount * sizeof(Vector3f));

	m_pVertsIndex = new unsigned int[trianglePoint];
	memcpy(m_pVertsIndex, vertsIndex, trianglePoint * sizeof(unsigned int));

	m_pVertsUVIndex = new Vector2f[m_iVertsCount];
	memcpy(m_pVertsUVIndex, uvIndex, m_iVertsCount * sizeof(Vector2f));

	m_iTotalVertsCount = trianglePoint;

	ConstructTriangles();
}

MeshTriangle::MeshTriangle(const std::string& fileName, const Point3& pos, const float scale):MeshTriangle()
{
	m_scale = scale;
	m_pos = pos;
	m_strObjName = fileName;
	ConstructTriangles(m_strObjName);
}


bool MeshTriangle::RayTrace(const Point3& pos, const Vector3f& dir, std::optional<TraceInfo>& traceInfo)
{
	float fracion = 0, maxFraction = std::numeric_limits<float>::max();
	Vector2f st;
	bool bTrace(false);
	unsigned triangleIndex = 0;

	for (unsigned i=0;i<m_vecTriangles.size();i++)
	{
		auto pTriangle = m_vecTriangles[i];
		if (pTriangle->RayTrace(pos, dir, traceInfo) && traceInfo->Fraction < maxFraction)
		{
			bTrace = true;
			maxFraction = fracion;
			triangleIndex = i;
		}
	}

	if (bTrace)
	{
		traceInfo->TriangleIndex = triangleIndex;
	}

	return bTrace;
}

bool MeshTriangle::RayTraceWithBVH(const Point3& pos, const Vector3f& dir, std::optional<TraceInfo>& traceInfo)
{
	if (m_pBVH)
	{
		return m_pBVH->RayTrace(pos, dir, traceInfo);
	}

	return false;
}

bool MeshTriangle::GetSurfaceProperty(const std::optional<TraceInfo>& traceInfo, SurfaceProperty& proerty)
{
	if (traceInfo->TriangleIndex >= m_vecTriangles.size())
	{
		return false;
	}

	auto pTriangle = m_vecTriangles[traceInfo->TriangleIndex];

	Vector3f normal;
	Vector2f uv;
	if (pTriangle->GetUVAndNormal(traceInfo->ST,normal,uv))
	{
		proerty.normal = normal;
		proerty.uv = uv;

		return true;
	}

	return false;
}

void MeshTriangle::Sample(Vector3f& pos, Vector3f& normal, float& pdf)
{
	/*
	//这里就随机一个三角形
	int triangleCount = m_vecTriangles.size();
	triangleCount = (int)Get_random_float(0,triangleCount);
	if (triangleCount <0 || triangleCount >= m_vecTriangles.size())
	{
		triangleCount = 0;
	}

	auto& triangle = m_vecTriangles[triangleCount];
	triangle->Sample(pos, normale, pdf);
	*/
	m_pBVH->Sample(pos, normal, pdf);
}

bool MeshTriangle::HasEmit()
{
	return GetObjectMaterial()->HasEmission();
}

Color3 MeshTriangle::GetDiffuseColor(const Vector2f& uv, const int matIndex)
{
	if (matIndex == -1)
	{
		return Vector3f(0.5, 0.5, 0.5);
		/*
		float scale = 5;
		float pattern = (fmodf(uv.x * scale, 1) > 0.5) ^ (fmodf(uv.y * scale, 1) > 0.5);
		return Lerp(Vector3f(0.815, 0.235, 0.031), Vector3f(0.937, 0.937, 0.231), pattern);
		*/
	}
	else {
		return 0;
	}
	
}

void MeshTriangle::ConstructTriangles()
{
	auto min_pos = Vector3f(std::numeric_limits<float>::infinity());
	auto max_pos = Vector3f(-std::numeric_limits<float>::infinity());

	for (unsigned i=0;i<m_iTriangleCount;i++)
	{
		std::shared_ptr<Triangle> pTriangle = std::make_shared<Triangle>(this);
		for (unsigned j=0;j<3;j++)
		{
			unsigned index = i * 3 + j;
			unsigned vertsIndex = m_pVertsIndex[index];
			pTriangle->Point[j] = m_pVerts[vertsIndex];
			pTriangle->UVIndex[j] = m_pVertsUVIndex[vertsIndex];

			min_pos = Min(min_pos, pTriangle->Point[j]);
			max_pos = Max(max_pos, pTriangle->Point[j]);
		}
		pTriangle->CalcTriangleNormalAndArea();
		pTriangle->BuildBoundingBox();
		pTriangle->MatIndex = 0;
		m_area += pTriangle->GetArea();
		m_vecTriangles.push_back(pTriangle);
	}

	if (m_pBoundingBox == nullptr)
	{
		m_pBoundingBox = std::make_shared<BoundingBox>(min_pos, max_pos);
	}

	std::vector<Object*> vecObj;
	for (auto obj:m_vecTriangles)
	{
		vecObj.push_back(obj.get());
	}

	m_pBVH = std::make_shared<BVH>(vecObj);
}

std::shared_ptr<Material> MakeMatByOBJ(const std::optional<OBJL::Material>& mat)
{
	std::shared_ptr<Material> matPtr = std::make_shared<MatLambert>(Color3(1));
	if (mat)
	{
		matPtr->m_diffuseMap = mat->map_Kd;
		matPtr->m_bumpMap = mat->map_bump;
	}

	return matPtr;
}

void MeshTriangle::ConstructTriangles(const std::string& fileName)
{
	OBJL::Loader objLoader;
	if (!objLoader.LoadFile(fileName))
	{
		m_vecTriangles.clear();
		m_objMatVec.clear();
		return;
	}

	auto min_pos = Vector3f::MakeMax();
	auto max_pos = Vector3f::MakeMin();

	for (int meshIndex =0;meshIndex <objLoader.LoadedMeshes.size();meshIndex++)
	{
		//auto& mesh = objLoader.LoadedMeshes[meshIndex];
		//auto mat = MakeMatByOBJ(mesh.MeshMaterial);
		//m_objMatVec.push_back(mat);

		///这个地方可以创建纹理
		for (int i=0;i<objLoader.LoadedIndices.size();i+=3)
		{
			std::shared_ptr<Triangle> pTriangle = std::make_shared<Triangle>(this);
			for (int j=0;j<3;j++)
			{
				auto vertIndex = objLoader.LoadedIndices[i + j];
				auto& loaderVert = objLoader.LoadedVertices[vertIndex];
				pTriangle->Point[j] = Point3(loaderVert.Position.X, loaderVert.Position.Y, loaderVert.Position.Z);
				pTriangle->PointNoraml[j] = Vector3f(loaderVert.Normal.X, loaderVert.Normal.Y, loaderVert.Normal.Z);
				pTriangle->UVIndex[j] = Vector2f(loaderVert.TextureCoordinate.X, loaderVert.TextureCoordinate.Y);
				
				pTriangle->Point[j] *= m_scale;
				pTriangle->Point[j] += m_pos;

				min_pos = Min(min_pos, pTriangle->Point[j]);
				max_pos = Max(max_pos, pTriangle->Point[j]);
			}
			pTriangle->CalcTriangleNormalAndArea();
			pTriangle->BuildBoundingBox();
			pTriangle->MatIndex = meshIndex;
			m_area += pTriangle->GetArea();
			m_vecTriangles.push_back(pTriangle);
		}
	}

	if (m_pBoundingBox == nullptr)
	{
		m_pBoundingBox = std::make_shared<BoundingBox>(min_pos, max_pos);
	}

	std::vector<Object*> vecObj;
	for (auto obj : m_vecTriangles)
	{
		vecObj.push_back(obj.get());
	}

	m_pBVH = std::make_shared<BVH>(vecObj);
}

