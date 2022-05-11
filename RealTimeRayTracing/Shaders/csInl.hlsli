#ifndef __CSINL__
#define __CSINL__

#define M_ININITY 0xFFFFFFF
#define M_PI 3.14159265358979323846
#define ZERO_FLOAT_2 float2(0, 0)
#define ZERO_FLOAT_3 float3(0, 0,0)

static const uint MAT_LAMBERT = 1;

struct Material
{
    uint Type;
    float3 Albedo;
    float3 Emission;
};

struct BoundingBox
{
    float3 MinVert;
    float3 MaxVert;
    float3 CenterVert;
};

struct Sphere
{
    float3 Center;
    float Radius;
    Material Mat;
};

struct Triangle
{
    int Index;
    float Area;
    float3 Point[3];
    float3 PointNoraml[3];
    float2 UVIndex[3];
    float3 TriangleNormal;
    BoundingBox AABB;
    Material Mat;
};

struct BVHNode
{
    BoundingBox AABB;
    int TriangleIndex;
    int NodeIndex;
    int ParentNodeIndex;
    int RightBrotherNodeIndex;
    int LeftNodeIndex;
    int RightNodeIndex;
};

struct Ray
{
    float3 OrgPos;
    float3 Dir;
};

struct TraceInfo
{
    bool BTraced;
    float Fraction;
    float2 UV;
    float2 ST;
    float3 HitNormal;
    float3 HitPoint;
    float3 Radiance;
    Material Mat;
};

void InitTraceInfo(inout TraceInfo traceInfo)
{
    Material mat;
    mat.Albedo = ZERO_FLOAT_3;
    mat.Emission = ZERO_FLOAT_3;
    mat.Type = MAT_LAMBERT;
    
    traceInfo.BTraced = false;
    traceInfo.Fraction = M_ININITY;
    traceInfo.UV = ZERO_FLOAT_2;
    traceInfo.ST = ZERO_FLOAT_2;
    traceInfo.HitNormal = ZERO_FLOAT_3;
    traceInfo.HitPoint = ZERO_FLOAT_3;
    traceInfo.Radiance = ZERO_FLOAT_3;
    traceInfo.Mat = mat;
}

Ray GenerateCamRay(uint2 pixel,float2 offset,float2 dimensions,float3 camPos,float4x4 vpInvert)
{
    float2 pixelIndex = float2(pixel.xy) + offset;
    
    //NDC [-1,1]
    float2 screenPos = pixelIndex / dimensions * 2.0 - 1.0;
    screenPos.y = -screenPos.y;
    
    float4 worldPos = mul(vpInvert, float4(screenPos, 0, 1));
    
    //这里一定要在除以W
    //https://feepingcreature.github.io/math.html
    worldPos.xyz /= worldPos.w;
    
    float3 dir = normalize(worldPos.xyz - camPos);
    
    Ray ray;
    ray.OrgPos = camPos;
    ray.Dir = dir;

    return ray;
}

void Swap(inout float a, inout float b)
{
    float tmp = a;
    a = b;
    b = tmp;
}

float Max(float a, float b, float c)
{
    float max = a;
    if (b > a)
        max = b;
    if(c > max)
        max = c;
    return max;
}

float Min(float a, float b, float c)
{
    float min = a;
    if (b < a)
        min = b;
    if (c < min)
        min = c;
    return min;
}

bool TraceSphere(Ray ray,Sphere sphere,inout float fraction)
{
    float3 pos = ray.OrgPos;
    float3 dir = ray.Dir;
    
    float3 l = pos - sphere.Center;
    float a = dot(dir, dir);
    float b = 2 * dot(l, dir);
    float c = dot(l, l) - sphere.Radius * sphere.Radius;

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
    else
    {
        float sqrDt = sqrt(dt);
        float calA = -0.5 / a;
        float result1 = calA * (b + sqrDt);
        float result2 = calA * (b - sqrDt);

        if (result1 < 0 && result2 >= 0)
        {
            t = result2;
        }
        else if (result1 >= 0 && result2 < 0)
        {
            t = result1;
        }
        else if (result1 >= 0 && result2 >= 0)
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
    }

    if (t < 0.2)
    {
        return false;
    }

    fraction = t;
    return true;
}

bool TraceAABB(Ray ray, BoundingBox aabb, inout float fraction)
{
    float3 pos = ray.OrgPos;
    float3 dir = ray.Dir;
    float invX = 1.0f / dir.x;
    float invY = 1.0f / dir.y;
    float invZ = 1.0f / dir.z;

    float tMinX = (aabb.MinVert.x - pos.x) * invX;
    float tMaxX = (aabb.MaxVert.x - pos.x) * invX;

    float tMinY = (aabb.MinVert.y - pos.y) * invY;
    float tMaxY = (aabb.MaxVert.y - pos.y) * invY;

    float tMinZ = (aabb.MinVert.z - pos.z) * invZ;
    float tMaxZ = (aabb.MaxVert.z - pos.z) * invZ;

    if (dir.x < 0)
    {
       Swap(tMinX, tMaxX);
    }

    if (dir.y < 0)
    {
        Swap(tMinY, tMaxY);
    }

    if (dir.z < 0)
    {
        Swap(tMinZ, tMaxZ);
    }

    float tEnter = Max(tMinX, tMinY, tMinZ);
    float tExit = Min(tMaxX, tMaxY, tMaxZ);

	//注意 有tEnter==tExit的情况
    if (tEnter <= tExit && tExit >= 0)
    {
        fraction = tEnter;
        return true;
    }

    return false;
}

bool TraceTriangle(Ray ray, Triangle trian, inout float fraction)
{
    float3 pos = ray.OrgPos;
    float3 dir = ray.Dir;
    
    if (dot(trian.TriangleNormal, dir) > 0)
    {
        return false;
    }

    float3 p0 = trian.Point[0];
    float3 p1 = trian.Point[1];
    float3 p2 = trian.Point[2];
    float3 E1 = p1 - p0;
    float3 E2 = p2 - p0;
    float3 S = pos - p0;
    float3 S1 = cross(dir, E2);
    float3 S2 = cross(S, E1);

    float division = dot(S1, E1);
    division = 1.0f / division;

    float t = dot(S2, E2) * division;
    float b1 = dot(S1, S) * division;
    float b2 = dot(S2, dir) * division;

    float b = 1 - b1 - b2;
    if (t > 0 && b1 > 0 && b2 > 0 && b > 0)
    {
        fraction = t;
        //st.x = b1;
       // st.y = b2;
        return true;
    }

    return false;
}

void GetObjectSurfaceProerty(Sphere sphere,TraceInfo traceInfo, inout float3 normal)
{
    normal = normalize(traceInfo.HitPoint - sphere.Center);
}

void GetObjectSurfaceProerty(Triangle trian,TraceInfo traceInfo, inout float3 normal)
{
    normal = trian.TriangleNormal;
}

bool IsRootNode(BVHNode node)
{
    return (node.ParentNodeIndex == -1);
}

bool IsLeafNode(BVHNode node)
{
    return node.LeftNodeIndex == -1 && node.RightBrotherNodeIndex == -1;
}

int GetNodeRightBrotherNodeIndex(BVHNode node)
{
    return node.RightBrotherNodeIndex;
}
#endif