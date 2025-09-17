#include "Shared.hlsli"

//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
PS_INPUT_SKYBOX VS_Skybox(VS_INPUT_SKYBOX input)
{
    // HLSL에서는 열 우선 행렬이다.
    PS_INPUT_SKYBOX output = (PS_INPUT_SKYBOX) 0;

    output.Pos = mul(input.Pos, SkyboxView);
    output.Pos = mul(output.Pos, SkyboxProjection);
    output.Tex = input.Pos.xyz;
    
    return output;
}