/*****************************************************************************
* @File : D:\MyGit\RayTracingManual\RayTracingManual\WhittedStyleRayTracing\Global.h
* @Author : Wang ZhiHao
* @Date : 2021/9/14 15:12
*****************************************************************************/

#pragma once

#include <iostream>
#include <random>
#include "Vec.h"
#include "BoundingBox.h"

#define M_PI 3.14159265358979323846

inline Vector3f Max(const Vector3f& v1, const Vector3f& v2)
{
	return Vector3f(std::max(v1.x, v2.x), std::max(v1.y, v2.y), std::max(v1.z, v2.z));
}

inline Vector3f Min(const Vector3f& v1, const Vector3f& v2)
{
	return Vector3f(std::min(v1.x, v2.x), std::min(v1.y, v2.y), std::min(v1.z, v2.z));
}

inline BoundingBox Union(const BoundingBox& box1, const BoundingBox& box2)
{
	auto min_vert = Min(box1.m_MinVert, box2.m_MinVert);
	auto max_vert = Max(box1.m_MaxVert, box2.m_MaxVert);

	BoundingBox unionBox(min_vert, max_vert);
	return unionBox;
}

inline BoundingBox Union(const BoundingBox& box1, const Vector3f& vec)
{
	auto min_vert = Min(box1.m_MinVert, vec);
	auto max_vert = Max(box1.m_MaxVert, vec);

	BoundingBox unionBox(min_vert, max_vert);
	return unionBox;
}

inline float Clamp(const float min, const float max, const float value)
{
	return std::max(min, std::min(max, value));
}

inline float Get_random_float(float from = 0.0f,float to = 1.0f)
{
	std::random_device dev;
	std::mt19937 rng(dev());
	std::uniform_real_distribution<float> dist(from, to); // distribution in range [1, 6]

	return dist(rng);
}

inline float Degrees_to_Radians(float degrees)
{
	return degrees * M_PI / 180;
}

inline float Radians_to_Degrees(float radians)
{
	return radians * 180 / M_PI;
}

inline Vector3f Reflect(const Vector3f& wi, const Vector3f& N)
{
	return wi - 2 * DotProduct(wi, N) * N;
}

inline Vector3f Refract(const Vector3f& l, const Vector3f& N, const float refractivity)
{
	/*
	float cosi = Clamp(-1, 1, DotProduct(l, N));
	float etai = 1, etat = refractivity;
	Vector3f n = N;
	if (cosi < 0) { cosi = -cosi; }
	else { std::swap(etai, etat); n = -N; }
	float eta = etai / etat;
	float k = 1 - eta * eta * (1 - cosi * cosi);
	return k < 0 ? 0 : eta * l + (eta * cosi - sqrtf(k)) * n;
	*/
	
	float etal = 1;
	float etat = refractivity;

	Vector3f n = N;
	float cosl = Clamp(-1, 1, DotProduct(l, N));
	if (cosl < 0)
	{
		cosl = -cosl;
	}
	else {
		std::swap(etat, etal);
		n = -N;
	}
	float l_over_t = etal / etat;
	float k = 1 - l_over_t * l_over_t * (1 - cosl * cosl);
	if (k < 0)
	{
		return 0;
	}

	return l_over_t * l + n * (l_over_t * cosl - std::sqrtf(k));

}

inline float Fresnel(const Vector3f & view,const Vector3f& N,const float refractivity)
{
	auto etai = 1.0f; auto etat = refractivity;
	auto norView = view.Nor();
	auto cosi = Clamp(-1,1,DotProduct(norView, N));

	if (cosi > 0)
	{
		std::swap(etai, etat);
	}
	
	auto sint = etai / etat * std::sqrtf(std::max(0.0f, 1 - cosi * cosi));
	if (sint > 1)
	{
		return 1;
	}

	//Schlick's Approximation
	/*
	auto r0 = (etai - etat) / (etai + etat);
	r0 = r0 * r0;
	auto rthea = r0 + (1 - r0) * std::powf(1 + cosi, 5);
	return rthea;
	*/
	
	//Total Formula
	float cost = sqrtf(std::max(0.f, 1 - sint * sint));
	cosi = fabsf(cosi);
	float Rs = ((etat * cosi) - (etai * cost)) / ((etat * cosi) + (etai * cost));
	float Rp = ((etai * cosi) - (etat * cost)) / ((etai * cosi) + (etat * cost));
	return (Rs * Rs + Rp * Rp) / 2;
}

inline Vector3f FromHemisphereToWorld(const Vector3f& local, const Vector3f& N)
{
	Vector3f T, B;
	if (std::fabs(N.x) > std::fabs(N.y))
	{
		float len = 1.0f / std::sqrt(N.x * N.x + N.z * N.z);
		B = Vector3f(N.z * len, 0.0, -N.x * len);
	}
	else {
		float len = 1.0f / std::sqrt(N.y * N.y + N.z * N.z);
		B = Vector3f(0.0, N.z * len, -N.y * len);
	}
	T = CrossProduct(B, N);
	return local.x * T + local.y * B + local.z * N;
}

inline Vector3f HemisphereUniformSample(const Vector3f& wo, const Vector3f& N)
{
	
	float kesi1 = Get_random_float();
	float kesi2 = Get_random_float();

	float z = 1 - kesi1;
	float kesiTmp1 = std::sqrt(1 - z * z);
	float phi = 2 * M_PI * kesi2;
	float x = std::cos(phi) * kesiTmp1;
	float y = std::sin(phi) * kesiTmp1;

	Vector3f local(x, y, z);
	return FromHemisphereToWorld(local, N);
	/*
	float x_1 = Get_random_float(), x_2 = Get_random_float();
	float z = std::fabs(1.0f - 2.0f * x_1);
	float r = std::sqrt(1.0f - z * z), phi = 2 * M_PI * x_2;
	Vector3f localRay(r * std::cos(phi), r * std::sin(phi), z);
	return FromHemisphereToWorld(localRay, N);
	*/
}
