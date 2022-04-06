/*****************************************************************************
* @File : D:\MyGit\RayTracingManual\RayTracingManual\WhittedStyleRayTracing\Vec.h
* @Author : Wang ZhiHao
* @Date : 2021/9/14 14:15
*****************************************************************************/

#pragma once
#include <iostream>

class Vector2f
{
public:
	float x = 0;
	float y = 0;

public:
	Vector2f() {}
	Vector2f(float xx):x(xx),y(xx){}
	Vector2f(float xx,float yy):x(xx),y(yy){}
	Vector2f(const Vector2f& vec)
	{
		x = vec.x; y = vec.y;
	}
	
	Vector2f operator*(const float& r) const
	{
		return Vector2f(x * r, y * r);
	}
	Vector2f operator+(const Vector2f& v) const
	{
		return Vector2f(x + v.x, y + v.y);
	}

};

class Vector3f
{
public:
	float x = 0; 
	float y = 0;
	float z = 0;

	Vector3f() {}
	Vector3f(float xx,float yy,float zz):x(xx),y(yy),z(zz){}
	Vector3f(float xx):x(xx),y(xx),z(xx){}
	Vector3f(const Vector3f& vec)
	{
		x = vec.x; y = vec.y; z = vec.z;
	}

public:
	Vector3f operator * (const Vector3f& vec) const
	{
		return Vector3f(x * vec.x, y * vec.y, z * vec.z);
	}

	Vector3f operator * (const float r) const
	{
		return Vector3f(x * r, y * r, z * r);
	}

	Vector3f operator / (const float r) const
	{
		return Vector3f(x / r, y / r, z / r);
	}

	Vector3f operator - (const Vector3f& vec) const
	{
		return Vector3f(x - vec.x, y - vec.y, z - vec.z);
	}

	Vector3f operator + (const Vector3f& vec) const
	{
		return Vector3f(x + vec.x, y + vec.y, z + vec.z);
	}

	Vector3f operator -() const
	{
		return Vector3f(-x, -y, -z);
	}

	Vector3f& operator +=(const Vector3f& vec)
	{
		x += vec.x; y += vec.y; z += vec.z;
		return *this;
	}

	Vector3f& operator *=(const float r)
	{
		x *= r; y *= r; z *= r;
		return *this;
	}

	friend Vector3f operator *(const float r, const Vector3f& vec)
	{
		return Vector3f(r * vec.x, r * vec.y, r * vec.z);
	}

public:
	void Normalize()
	{
		float length = x * x + y * y + z * z;
		if (length > 0)
		{
			length =1 / std::sqrtf(length);
			x*= length; y*= length; z*= length;
		}
	}

	Vector3f Nor() const
	{
		float length = x * x + y * y + z * z;
		if (length > 0)
		{
			length = 1 / std::sqrtf(length);
			return Vector3f(x * length, y * length, z * length);
		}
		return *this;
	}
	
	bool IsZero()const
	{
		return x == 0 && y == 0 && z == 0;
	}

	float DotProduct(const Vector3f& vec) const
	{
		return x * vec.x + y * vec.y + z * vec.z;
	}

	Vector3f CrossProduct(const Vector3f& vec) const
	{
		return Vector3f(y * vec.z - z * vec.y, z * vec.x - x * vec.z, x * vec.y - y * vec.x);
	}

	float Len() const
	{
		return std::sqrtf(x * x + y * y + z * z);
	}
	float LenSquared() const
	{
		return x * x + y * y + z * z;
	}

	static Vector3f MakeMax() {
		return Vector3f(std::numeric_limits<float>::max());
	}
	static Vector3f MakeMin() {
		return Vector3f(-std::numeric_limits<float>::max());
	}
};

using Point3 = Vector3f;
using Color3 = Vector3f;

inline Vector3f Lerp(const Vector3f& a, const Vector3f& b, const float t)
{
	return a * (1 - t) + b * t;
}

inline float DotProduct(const Vector3f& a, const Vector3f& b)
{
	return a.x * b.x + a.y * b.y + a.z * b.z;
}

inline Vector3f CrossProduct(const Vector3f& a, const Vector3f& b)
{
	return Vector3f(a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x);
}

inline Vector3f Normalize(const Vector3f& vec)
{
	float length = vec.x * vec.x + vec.y * vec.y + vec.z * vec.z;
	if (length > 0)
	{
		length = 1 / std::sqrtf(length);
		return Vector3f(vec.x * length, vec.y * length, vec.z * length);
	}
	return vec;
}
