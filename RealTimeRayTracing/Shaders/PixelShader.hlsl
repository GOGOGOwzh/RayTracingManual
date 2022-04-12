
Texture2D ResultTexture : register(t0);

SamplerState SamplerWrap;

struct V2P
{
    float4 Position : SV_POSITION;
    float2 UV : TEXCOORD0;
};

float4 PS_main(V2P i) : SV_TARGET
{
    float3 color = ResultTexture.Sample(SamplerWrap, i.UV).rgb;
    return float4(color, 1.0f);
}