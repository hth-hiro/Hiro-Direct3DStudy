#include "Shared.hlsli"

//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
PS_INPUT_SKYBOX VS_Skybox(VS_INPUT_SKYBOX input)
{
    // HLSL에서는 열 우선 행렬이다.
    PS_INPUT_SKYBOX output = (PS_INPUT_SKYBOX) 0;

    float4 pos = float4(input.Pos.xyz, 1.0f);

    float4x4 viewNoTranslation = SkyboxView;
    viewNoTranslation._41 = 0.0f;
    viewNoTranslation._42 = 0.0f;
    viewNoTranslation._43 = 0.0f;
    
    output.Pos = mul(pos, viewNoTranslation);
    output.Pos = mul(output.Pos, SkyboxProjection);
    output.Tex = input.Pos.xyz;
    
    return output;
}