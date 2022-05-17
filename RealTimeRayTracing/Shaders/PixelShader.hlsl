
cbuffer CB : register(b0)
{
    float4x4 VPInvert;
    float3 CameraPos;
    
    uint SampleCount;
    uint Timestamp;
};

Texture2D ResultTexture : register(t0);

SamplerState SamplerWrap;

struct V2P
{
    float4 Position : SV_POSITION;
    float2 UV : TEXCOORD0;
};

float3 LinearToSrgb(float3 c)
{
    float3 sq1 = sqrt(c);
    float3 sq2 = sqrt(sq1);
    float3 sq3 = sqrt(sq2);
    float3 srgb = 0.662002687 * sq1 + 0.684122060 * sq2 - 0.323583601 * sq3 - 0.0225411470 * c;
    return srgb;
}

float3 ACESToneMapping(float3 color, float adaptedLum)
{
    const static float A = 2.51f;
    const static float B = 0.03f;
    const static float C = 2.43f;
    const static float D = 0.59f;
    const static float E = 0.14f;

    color *= adaptedLum;
    return (color * (A * color + B)) / (color * (C * color + D) + E);
}

float4 PS_main(V2P i) : SV_TARGET
{
    float3 color = ResultTexture.Sample(SamplerWrap, i.UV).rgb;
    
   // color = ACESToneMapping(color, 1);
   // color = LinearToSrgb(color);
    return float4(color, 1.0 / (SampleCount + 1.0));
}