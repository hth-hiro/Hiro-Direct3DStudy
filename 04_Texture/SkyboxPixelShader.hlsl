#include "Shared.hlsli"

//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 PS_Skybox(PS_INPUT_SKYBOX input) : SV_Target
{
    float3 dir = normalize(input.Tex);

    return txCube.Sample(samCube, dir);
}