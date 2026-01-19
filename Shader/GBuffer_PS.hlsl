#include "Shared.hlsli"

//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
struct GBufferOut
{
    float4 BaseColor : SV_Target0;
    float4 Normal : SV_Target1;
    float4 Position : SV_Target2;
};

GBufferOut main(PS_INPUT_DEFERRED input)
{
    GBufferOut o;
    
    float4 texColor = txDiffuse.Sample(samLinear, input.uv);
    
    float4 color = texColor * GeometryColor;
    
    color.a = 1.0f;
    
    //o.BaseColor = texColor * GeometryColor;
    o.BaseColor = color;

    float3 n = normalize(input.normalWS);
    
    o.Normal = float4(EncodeNormal(n), 1.0f);

    o.Position = float4(input.positionWS, 1.0f);
   
    return o;
}