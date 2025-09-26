#include "Shared.hlsli"

//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 PS_Skybox(PS_INPUT_SKYBOX input) : SV_Target
{
    return txCube.Sample(samCube, input.Tex);
}