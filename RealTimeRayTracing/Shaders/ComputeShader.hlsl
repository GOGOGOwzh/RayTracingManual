#include "csInl.hlsli"

cbuffer CB : register(b0)
{
    float4x4 VPInvert;
    float3 CameraPos;
    
    uint SampleCount;
    uint Timestamp;
};

RWTexture2D<float4> ResultTexture : register(u0);
StructuredBuffer<Sphere> VecSpheres : register(t0);
StructuredBuffer<Triangle> VecTriagles : register(t1);
StructuredBuffer<BVHNode> VecBVH : register(t2);
StructuredBuffer<Triangle> VecTriaglesLight : register(t3);

#define MAX_SAMPLE_COUNT 2

static uint LightTriangleIndex = 0;

void SampleAreaLight(inout float3 pos, inout float3 normal, inout float pdf, float kesi1, float kesi2)
{
    uint triangleCount, triangleStride;
    VecTriaglesLight.GetDimensions(triangleCount, triangleStride);
    int chooseIndex = LightTriangleIndex % triangleCount;
    LightTriangleIndex++;
    
    Triangle tri = VecTriaglesLight[chooseIndex];
    
    SampleTriangle(tri, pos, normal, pdf, kesi1, kesi2);
}

void RayTrace(Ray ray,inout TraceInfo traceInfo)
{
    traceInfo.Fraction = M_ININITY;
    traceInfo.BTraced = false;
    
    float fraction = M_ININITY;
 
    bool bTraceSphere = false;
    bool bTraceTriangle = false;
    bool bTraceBVH = false;
    
    /*
    //Sphere
   
    Sphere sphere;
    uint sphereCount,stride,i;
    VecSpheres.GetDimensions(sphereCount, stride);
    for (i = 0; i < sphereCount;i++)
    {
        sphere = VecSpheres[i];
        if (TraceSphere(ray,sphere,fraction) && fraction < traceInfo.Fraction)
        {
            traceInfo.Fraction = fraction;
            bTraceSphere = true;
            
            traceInfo.Mat = sphere.Mat;
            traceInfo.HitPoint = ray.OrgPos + ray.Dir * fraction;
            GetObjectSurfaceProerty(sphere, traceInfo, traceInfo.HitNormal);
        }
    }
    */
    
    /*
    uint triangleCount, triangleStride, j;
    VecTriagles.GetDimensions(triangleCount, triangleStride);
    
    Triangle trian;
    for (j = 0; j < triangleCount; j++)
    {
        trian = VecTriagles[j];
        if (TraceTriangle(ray, trian, fraction) && fraction < traceInfo.Fraction)
        {
            traceInfo.Fraction = fraction;
            bTraceTriangle = true;
            
            traceInfo.Mat = trian.Mat;
            traceInfo.HitPoint = ray.OrgPos + ray.Dir * fraction;
            GetObjectSurfaceProerty(trian, traceInfo, traceInfo.HitNormal);
        }
    }
    */
    
    
    //BVH
    uint nodeCount, nodeStride, i;
    VecBVH.GetDimensions(nodeCount, nodeStride);
    BVHNode node;
    for (i = 0; i < nodeCount;i++)
    {
        node = VecBVH[i];
        if (node.TriangleIndex >= 0)
        {
            Triangle trian = VecTriagles[node.TriangleIndex];
            if (TraceTriangle(ray, trian, fraction) && fraction < traceInfo.Fraction)
            {
                traceInfo.Fraction = fraction;
                bTraceBVH = true;
            
                traceInfo.Mat = trian.Mat;
                traceInfo.HitPoint = ray.OrgPos + ray.Dir * fraction;
                GetObjectSurfaceProerty(trian, traceInfo, traceInfo.HitNormal);
            }
        }
        else if (TraceAABB(ray,node.AABB,fraction) && fraction < traceInfo.Fraction)
        {
            continue;
        }
        else
        {
            //root node
            if (IsRootNode(node))
            {
                break;
            }
            
            if (node.RightBrotherNodeIndex != -1)
            {
                i = node.RightBrotherNodeIndex-1;
            }
        }
    }
    
    traceInfo.BTraced = bTraceSphere || bTraceTriangle ||bTraceBVH;
}

bool ShadringHitPoint(inout Ray ray,inout TraceInfo traceInfo)
{
    if(traceInfo.BTraced)
    {
        float3 hitPos = traceInfo.HitPoint;
        float3 brdfDir = float3(0, 0, 0);
        
        Material hitMat = traceInfo.Mat;
        uint matType = hitMat.Type;
        if (matType == MAT_MC_DIFFUSE)
        {
            if (IsEmissionMat(hitMat))
            {
                traceInfo.Radiance = hitMat.Emission;
                return true;
            }
            
            float3 lightColorDir = float3(0, 0, 0);
            float3 lightColorInDir = float3(0, 0, 0);
            
            float3 hitNor = traceInfo.HitNormal;
            
            float3 sampleAreaLightPos = float3(0, 0, 0);
            float3 sampleAreaLightNor = float3(0, 0, 0);
            float sampleAreaLightPDF = 0.00001f;
            float kesi1 = RandNext(traceInfo.Seed);
            float kesi2 = RandNext(traceInfo.Seed);
            SampleAreaLight(sampleAreaLightPos, sampleAreaLightNor, sampleAreaLightPDF, kesi1, kesi2);
            
            float3 wi = normalize(sampleAreaLightPos - hitPos);
            float3 wo = normalize(-ray.Dir);

            Ray testLightRay = MakeRay(hitPos, wi);
            TraceInfo testLightTraceInfo = InitTraceInfo();
            testLightTraceInfo.Seed = traceInfo.Seed;
    
            RayTrace(testLightRay, testLightTraceInfo);
            if (testLightTraceInfo.BTraced && IsEmissionMat(testLightTraceInfo.Mat))
            {
                Material lightMat = testLightTraceInfo.Mat;
                float len = length(sampleAreaLightPos-hitPos);
                float distance2 = len * len;
                lightColorDir = lightMat.Emission * BRDF(hitMat, wi, wo, hitNor) * dot(wi, hitNor) * dot(-wi, sampleAreaLightNor) / distance2 / sampleAreaLightPDF;
            }
            
            kesi1 = RandNext(traceInfo.Seed);
            kesi2 = RandNext(traceInfo.Seed);
            wi = normalize(HemisphereUniformSample(wo, hitNor, kesi1, kesi2));
            
            Ray testObjectRay = MakeRay(hitPos, wi);
            TraceInfo testObjectTraceInfo = InitTraceInfo();
            testObjectTraceInfo.Seed = traceInfo.Seed;
            
            RayTrace(testObjectRay, testObjectTraceInfo);
            if (testObjectTraceInfo.BTraced && !IsEmissionMat(testObjectTraceInfo.Mat))
            {
                lightColorInDir = BRDF(hitMat, wi, wo, hitNor) * dot(wi, hitNor) / PDF(hitMat, wi, wo, hitNor);
            }
            
            traceInfo.Attenuation *= (lightColorDir + lightColorInDir);
            traceInfo.Radiance += traceInfo.Attenuation;
            
            brdfDir = wi;
        }
        
        ray.Dir = brdfDir;
        ray.OrgPos = hitPos;
        return true;
    }
    else
    {
       // float t = 0.5 * (ray.Dir.y + 1.0);
       // float3 color = (1.0 - t) * float3(1.0, 1.0, 1.0) + t * float3(0.5, 0.7, 1.0);
        float3 color = float3(0, 0, 0);
        traceInfo.Radiance += color;
    }
    
    return true;
}


[numthreads(16, 16, 1)]
void CS_main( uint3 DTid : SV_DispatchThreadID )
{
    uint screen_width, screen_height;
    ResultTexture.GetDimensions(screen_width, screen_height);
    float2 dimensions = float2(screen_width, screen_height);
    
    uint2 pixel = DTid.xy;
    
    uint randSeed = RandInit(DTid.x + DTid.y * dimensions.x, Timestamp);
    
   
    float3 radiance = float3(0, 0, 0);
    for (uint j = 0; j < MAX_SAMPLE_COUNT; j++)
    {
        float2 offset = float2(RandNext(randSeed), RandNext(randSeed));
  
        Ray ray = GenerateCamRay(pixel, offset, dimensions, CameraPos, VPInvert);
  
        TraceInfo traceInfo = InitTraceInfo();
        traceInfo.Seed = randSeed;
        
        RayTrace(ray, traceInfo);
        if (!ShadringHitPoint(ray, traceInfo))
        {
            break;
        }
        
        randSeed = traceInfo.Seed;
        
        radiance += traceInfo.Radiance;

    }

    ResultTexture[DTid.xy] = float4(radiance / MAX_SAMPLE_COUNT, 1);
}