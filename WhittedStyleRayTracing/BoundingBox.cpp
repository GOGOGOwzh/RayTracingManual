#include "BoundingBox.h"
#include "Sphere.h"
#include "Global.h"
#include "TraceInfo.h"
#include "MeshTriangle.h"
#include "Triangle.h"

BoundingBox::BoundingBox(const Vector3f& minVert, const Vector3f& maxVert):BoundingBox()
{
	m_MinVert = Min(minVert, maxVert);
	m_MaxVert = Max(minVert, maxVert);

	m_CenterVert = (m_MaxVert + m_MinVert) * 0.5f;
}

BoundingBox::BoundingBox(const Sphere* pShere):BoundingBox()
{
	Point3 center = pShere->GetCenter();
	float radius = pShere->GetRadius();

	m_MinVert = Vector3f(center.x - radius, center.y - radius, center.z - radius);
	m_MaxVert = Vector3f(center.x + radius, center.y + radius, center.z + radius);

	m_CenterVert = (m_MaxVert + m_MinVert) * 0.5f;
}

BoundingBox::BoundingBox(const Triangle* pTriangle) :BoundingBox()
{
	m_MinVert = Min(pTriangle->Point[0], Min(pTriangle->Point[1], pTriangle->Point[2]));
	m_MaxVert = Max(pTriangle->Point[0], Max(pTriangle->Point[1], pTriangle->Point[2]));

	m_CenterVert = (m_MaxVert + m_MinVert) * 0.5f;
}

bool BoundingBox::OverLaps(const BoundingBox& box)
{
	bool x = box.m_MaxVert.x >= m_MinVert.x && box.m_MinVert.x <= m_MaxVert.x;
	bool y = box.m_MaxVert.y >= m_MinVert.y && box.m_MinVert.y <= m_MaxVert.y;
	bool z = box.m_MaxVert.z >= m_MinVert.z && box.m_MinVert.z <= m_MaxVert.z;

	return x && y && z;
}

bool BoundingBox::Inside(const Vector3f& vert)
{
	bool x = vert.x >= m_MinVert.x && vert.x <= m_MaxVert.x;
	bool y = vert.y >= m_MinVert.y && vert.y <= m_MaxVert.y;
	bool z = vert.z >= m_MinVert.z && vert.z <= m_MaxVert.z;

	return x && y && z;
}

bool BoundingBox::Trace(const Point3& pos, const Vector3f& dir, float& tenter, float& texit)
{
	float invX = 1.0f / dir.x;
	float invY = 1.0f / dir.y;
	float invZ = 1.0f / dir.z;

	float tMinX = (m_MinVert.x - pos.x) * invX;
	float tMaxX = (m_MaxVert.x - pos.x) * invX;

	float tMinY = (m_MinVert.y - pos.y) * invY;
	float tMaxY = (m_MaxVert.y - pos.y) * invY;

	float tMinZ = (m_MinVert.z - pos.z) * invZ;
	float tMaxZ = (m_MaxVert.z - pos.z) * invZ;

	if (dir.x < 0)
	{
		std::swap(tMinX, tMaxX);
	}

	if (dir.y < 0)
	{
		std::swap(tMinY, tMaxY);
	}

	if (dir.z < 0)
	{
		std::swap(tMinZ, tMaxZ);
	}

	float tEnter = std::max({ tMinX,tMinY,tMinZ });
	float tExit = std::min({ tMaxX,tMaxY,tMaxZ });

	tenter = tEnter;
	texit = tExit;

	//注意 有tEnter==tExit的情况
	if (tEnter <= tExit && tExit >= 0)
	{
		return true;
	}

	return false;
}

bool BoundingBox::RayTrace(const Point3& pos, const Vector3f& dir, std::optional<TraceInfo>& traceInfo)
{
	float tEnter;
	float tExit;
	bool bTrace = Trace(pos, dir, tEnter, tExit);
	traceInfo->Fraction = tEnter;

	return bTrace;
}

char BoundingBox::GetExType()
{
	auto diagonal = Diagonal();
	if (diagonal.x > diagonal.y && diagonal.x > diagonal.z)
	{
		return TYPE_X;
	}
	else if (diagonal.y >diagonal.z)
	{
		return TYPE_Y;
	}
	else {
		return TYPE_Z;
	}
}

BoundingBox BoundingBox::Clone()
{
	return BoundingBox(m_MinVert, m_MaxVert);
}

BoundingBox BoundingBox::Union(const BoundingBox& box)
{
	auto min_vert = Min(m_MinVert, box.m_MinVert);
	auto max_vert = Max(m_MaxVert, box.m_MaxVert);

	BoundingBox unionBox(min_vert, max_vert);
	return unionBox;
}

BoundingBox BoundingBox::Union(const Vector3f& vec)
{
	auto min_vert = Min(m_MinVert, vec);
	auto max_vert = Max(m_MaxVert, vec);

	BoundingBox unionBox(min_vert, max_vert);
	return unionBox;
}
