#include "csInl.hlsli"

cbuffer CB : register(b0)
{
    float4x4 VPInvert;
    float3 CameraPos;
    
    uint SampleCount;
};

RWTexture2D<float4> ResultTexture : register(u0);
StructuredBuffer<Sphere> VecSpheres : register(t0);
StructuredBuffer<Triangle> VecTriagles : register(t1);
StructuredBuffer<BVHNode> VecBVH : register(t2);

#define MAX_SAMPLE_DEPTH 1

void RayTrace(Ray ray,inout TraceInfo traceInfo)
{
    traceInfo.Fraction = M_ININITY;
    traceInfo.BTraced = false;
    
    float fraction = M_ININITY;
 
    /*
    //Sphere
    bool bTraceSphere = false;
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
    
    bool bTraceTriangle = false;
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
    
    bool bTraceBVH = false;
    uint nodeCount, nodeStride, i;
    VecBVH.GetDimensions(nodeCount, nodeStride);
    BVHNode node;
    for (i = 0; i < nodeCount;i++)
    {
        node = VecBVH[i];
        if (TraceAABB(ray,node.AABB,fraction) && fraction < traceInfo.Fraction)
        {
            if(node.TriangleIndex >= 0)
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
            else
            {
                if (node.ParentNodeIndex != -1 && !IsLeafNode(node))
                {
                    BVHNode parentNode = VecBVH[node.ParentNodeIndex];
                    if (parentNode.RightBrotherNodeIndex !=-1)
                    {
                        i = parentNode.RightBrotherNodeIndex - 1;
                    }
                }
            }
           
        }
    }
    
    traceInfo.BTraced = bTraceBVH;
}

bool ShadringHitPoint(inout Ray ray,inout TraceInfo traceInfo)
{
    if(traceInfo.BTraced)
    {
        Material mat = traceInfo.Mat;
        uint matType = mat.Type;
        if(matType == MAT_LAMBERT)
        {
            traceInfo.Radiance = mat.Albedo;
        }
        else
        {
            traceInfo.Radiance = mat.Albedo;
        }
        
        ray.Dir = ZERO_FLOAT_3;
        ray.OrgPos = ZERO_FLOAT_3;
        return true;
    }
    else
    {
        float t = 0.5 * (ray.Dir.y + 1.0);
        float3 color = (1.0 - t) * float3(1.0, 1.0, 1.0) + t * float3(0.5, 0.7, 1.0);
        traceInfo.Radiance = color;
    }
    
    return false;
}


[numthreads(16, 16, 1)]
void CS_main( uint3 DTid : SV_DispatchThreadID )
{
    uint screen_width, screen_height;
    ResultTexture.GetDimensions(screen_width, screen_height);
    float2 dimensions = float2(screen_width, screen_height);
    
    uint2 pixel = DTid.xy;
    float2 offset = float2(0.5, 0.5);
  
    Ray ray = GenerateCamRay(pixel, offset, dimensions, CameraPos, VPInvert);
  
    TraceInfo traceInfo;
    InitTraceInfo(traceInfo);
    
    for (uint i = 0; i < MAX_SAMPLE_DEPTH;i++)
    {
        RayTrace(ray, traceInfo);
        if (!ShadringHitPoint(ray,traceInfo))
        {
            break;
        }
    }
    
    ResultTexture[DTid.xy] = float4(traceInfo.Radiance, 1);
}