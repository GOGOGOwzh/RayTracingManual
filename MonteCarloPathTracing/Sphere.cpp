#include "Sphere.h"
#include "TraceInfo.h"
#include "Ray.h"
#include "BoundingBox.h"
#include "BVH.h"

Sphere::Sphere(const Point3& center, const float radius) :Object(TYPE_SPHERE)
{
	m_center = center;
	m_radius = radius;

	m_pBoundingBox = std::make_shared<BoundingBox>(this);
}

bool Sphere::RayTrace(const Point3& pos, const Vector3f& dir, std::optional<TraceInfo>& traceInfo)
{
	if (!CanTrace())
	{
		return false;
	}

	Vector3f l = pos - m_center;
	float a = DotProduct(dir, dir);
	float b = 2 * DotProduct(l, dir);
	float c = DotProduct(l, l) - m_radius * m_radius;

	float dt = b * b - 4 * a * c;
	float t = -1;

	if (dt < 0)
	{
		return false;
	}
	else if (dt == 0)
	{
		t = -b * 0.5 / a;
	}
	else {
		
		float sqrDt = std::sqrtf(dt);
		float calA = -0.5 / a;
		float result1 = calA * (b + sqrDt);
		float result2 = calA * (b - sqrDt);

		if (result1 <0 && result2 >=0)
		{
			t = result2;
		}
		else if (result1 >=0 && result2 <0)
		{
			t = result1;
		}
		else if (result1 >=0 && result2 >=0)
		{
			t = result1 > result2 ? result2 : result1;
		}
		else
		{
			return false;
		}
		
		if (t < 0)
		{
			t = result1;
		}
		if (t < 0)
		{
			return false;
		}
		
		
		
		/*
		float q = (b > 0) ? -0.5 * (b + std::sqrtf(dt)) : -0.5 * (b - std::sqrtf(dt));
		float t0 = q / a;
		float t1 = c / q;

		if (t0 > t1)
			std::swap(t0, t1);
		
		if (t0 < 0)
			t0 = t1;
		if (t0 < 0)
			return false;
		t = t0;
		*/
	
	}

	if (t < 0.5)
	{
		return false;
	}

	traceInfo->Fraction = t;

	return true;
}

bool Sphere::RayTraceWithBVH(const Point3& pos, const Vector3f& dir, std::optional<TraceInfo>& traceInfo)
{
	if (m_pBoundingBox->RayTrace(pos,dir, traceInfo))
	{
		traceInfo->HitObj = this;
		return RayTrace(pos, dir, traceInfo);
	}

	return false;
}

bool Sphere::GetSurfaceProperty(const std::optional<TraceInfo>& traceInfo, SurfaceProperty& proerty)
{
	if (traceInfo)
	{
		proerty.normal = Normalize(traceInfo->HitPoint - m_center);
		return true;
	}

	return false;
}
