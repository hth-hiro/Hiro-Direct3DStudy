#include "Shared.fxh"

//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 main(PS_INPUT input) : SV_Target
{
    float4 finalColor = {1.0,1.0,0.0,0.0};
    
    //float3 N = normalize(input.Norm);
    //float3 L = normalize(vLightDir.xyz);
    //finalColor.rgb += saturate(dot(N, L)) * vLightColor.xyz;
    
    finalColor += saturate(dot((float3) vLightDir, input.Norm) * vLightColor);
    
    finalColor.a = 1;
    return finalColor;
}