#include "../Shader/Shared.hlsli"

//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
PS_INPUT main(VS_INPUT input)
{
    PS_INPUT output = (PS_INPUT) 0;
    output.Pos = mul(input.Pos, World);
    
    output.WorldPos = output.Pos;
    
    output.Pos = mul(output.Pos, View);
    output.Pos = mul(output.Pos, Projection);

    output.Norm = normalize(mul(input.Norm, (float3x3) World));
    
    // 탄젠트와 비탄젠트를 PixelShader에 넘긴다.
    output.Tangent = normalize(mul(input.Tangent, (float3x3) World));
    output.Bitangent = normalize(mul(input.Bitangent, (float3x3) World));
    
    // 입력된 uv텍스처를 출력에 반영한다.
    output.Tex = input.Tex;

    return output;
}