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

struct Sphere
{
    float3 Center;
    float Radius;
    Material Mat;
};

struct Triangle
{
    float3 Pos[3];
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
    
    float4 worldPos = mul(vpInvert, float4(screenPos, 0, 1));
    //这里一定要在除以W
    worldPos.xyz /= worldPos.w;
    
    float3 dir = normalize(worldPos.xyz - camPos);
    
    Ray ray;
    ray.OrgPos = camPos;
    ray.Dir = dir;

    return ray;
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

void GetObjectSurfaceProerty(Sphere sphere,TraceInfo traceInfo, inout float3 normal)
{
    normal = normalize(traceInfo.HitPoint - sphere.Center);
}

void GetObjectSurfaceProerty(Triangle trian,TraceInfo traceInfo, inout float3 normal)
{
}

#endif