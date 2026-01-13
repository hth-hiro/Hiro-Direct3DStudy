#include "Shared.hlsli"

//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
PS_INPUT_DEFERRED main(VS_INPUT_DEFERRED input)
{
    PS_INPUT_DEFERRED output = (PS_INPUT_DEFERRED) 0;
    
    float4 posW = mul(float4(input.position, 1.0f), GeometryWorld);
    float4 posV = mul(posW, GeoMetryView);
    output.positionCS = mul(posV, GeoMetryProjection);
    
    float3 nW = normalize(mul(input.normal, (float3x3) GeometryWorld));
    output.normalWS = nW;
    output.positionWS = posW.xyz;
    output.uv = input.uv;
    
    return output;
}