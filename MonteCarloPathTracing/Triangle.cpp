#include "Triangle.h"
#include "TraceInfo.h"
#include "MeshTriangle.h"
#include "BoundingBox.h"
#include "Global.h"


/// <summary>
/// Triangle
/// </summary>

bool Triangle::GetUVAndNormal(const Vector2f& st, Vector3f& normal, Vector2f& uv)
{
	if (TriangleNormal.IsZero())
	{
		auto p0 = Point[0];	auto p1 = Point[1];	auto p2 = Point[2];
		auto E1 = p1 - p0; auto E2 = p2 - p0;

		normal = Normalize(CrossProduct(E1, E2));
		TriangleNormal = normal;
	}
	else {
		normal = TriangleNormal;
	}

	//重心坐标插值
	uv = UVIndex[0] * (1 - st.x - st.y) + UVIndex[1] * st.x + UVIndex[2] * st.y;

	return true;
}


bool Triangle::Trace(const Point3& pos, const Vector3f& dir, float& fraction, Vector2f& st)
{
	if (DotProduct(TriangleNormal,dir) > 0)
	{
		return false;
	}

	auto p0 = Point[0];	auto p1 = Point[1];	auto p2 = Point[2];
	auto E1 = p1 - p0; auto E2 = p2 - p0;
	auto S = pos - p0;
	auto S1 = CrossProduct(dir, E2);
	auto S2 = CrossProduct(S, E1);

	auto division = DotProduct(S1, E1);
	division = 1.0f / division;

	auto t = DotProduct(S2, E2) * division;
	auto b1 = DotProduct(S1, S) * division;
	auto b2 = DotProduct(S2, dir) * division;

	auto b = 1 - b1 - b2;
	if (t > 0 && b1 > 0 && b2 > 0 && b > 0)
	{
		fraction = t;
		st.x = b1;
		st.y = b2;
		return true;
	}

	return false;
}

void Triangle::CalcTriangleNormalAndArea()
{
	auto p0 = Point[0];	auto p1 = Point[1];	auto p2 = Point[2];
	auto E1 = p1 - p0; auto E2 = p2 - p0;

	auto E1XE2 = CrossProduct(E1, E2);
	TriangleNormal = Normalize(E1XE2);
	m_area = E1XE2.Len() * 0.5f;
}

void Triangle::BuildBoundingBox()
{
	m_pBoundingBox = std::make_shared<BoundingBox>(this);
}

bool Triangle::RayTrace(const Point3& pos, const Vector3f& dir, std::optional<TraceInfo>& traceInfo)
{
	float fraction = 0;
	Vector2f st;
	bool bTrace = Trace(pos, dir, fraction, st);
	if (bTrace)
	{
		traceInfo->Fraction = fraction;
		traceInfo->ST = st;
	}
	return bTrace;
}

bool Triangle::RayTraceWithBVH(const Point3& pos, const Vector3f& dir, std::optional<TraceInfo>& traceInfo)
{
	float fraction = 0;
	Vector2f st;
	bool bTrace = Trace(pos, dir, fraction, st);
	if (bTrace)
	{
		traceInfo->Fraction = fraction;
		traceInfo->ST = st;
		traceInfo->HitObj = this;
	}
	else {
		traceInfo->HitObj = nullptr;
	}

	return bTrace;
}

bool Triangle::GetSurfaceProperty(const std::optional<TraceInfo>& traceInfo, SurfaceProperty& proerty)
{
	Vector3f normal;
	Vector2f uv;
	if (GetUVAndNormal(traceInfo->ST, normal, uv))
	{
		proerty.normal = normal;
		proerty.uv = uv;

		return true;
	}

	return false;
}

std::shared_ptr<Material> Triangle::GetObjectMaterial(const unsigned char matIndex /*= 0*/)
{
	if (MatIndex >= 0 && PMesh)
	{
		return PMesh->GetObjectMaterial(MatIndex);
	}
	return nullptr;
}

void Triangle::Sample(Vector3f& pos, Vector3f& normal, float& pdf)
{
	float kesi1 = std::sqrt(Get_random_float());
	float kesi2 = Get_random_float();

	pos = (1.0f - kesi1) * Point[0] + kesi2 * kesi1 * Point[1] + kesi1 * (1.0f - kesi2) * Point[2];
	normal = TriangleNormal;
	pdf = 1.0f / m_area;
}
