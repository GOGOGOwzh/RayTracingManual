
struct VertextInput
{
    float4 position : POSITION;
    float2 tex : TEXCOORD0;
};

struct V2P
{
    float4 Position : SV_POSITION;
    float2 UV : TEXCOORD0;
};

V2P VS_main(VertextInput input)
{
    V2P o;
    o.Position = input.position;
    o.UV = input.tex;
    
    return o;
}