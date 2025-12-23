#include "Shared.hlsli"

//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
PS_INPUT_TONEMAP main(VS_INPUT_TONEMAP input)
{
    PS_INPUT_TONEMAP output = (PS_INPUT_TONEMAP) 0;
    
    output.Pos = float4(input.Pos.xy, 0.0f, 1.0f);
    output.Tex = input.Tex;
    
    return output;
}