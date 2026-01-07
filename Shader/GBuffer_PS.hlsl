#include "Shared.hlsli"

//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
struct GBufferOut
{
    float4 BaseColor : SV_Target20;
    float4 Normal : SV_Target21;
    float4 Position : SV_Target22;
};

GBufferOut main(PS_INPUT_DEFERRED input) : SV_Target
{
    GBufferOut o;
    
    o.BaseColor = GeometryColor;

    float3 n = normalize(input.normalWS);
    o.Normal = float4(EncodeNormal(n), 1.0f);

    o.Position = float4(input.positionWS, 1.0f);
   
    return o;
}