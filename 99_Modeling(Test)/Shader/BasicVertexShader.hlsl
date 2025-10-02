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

    output.Norm = input.Norm;

    output.Tangent = input.Tangent;

    output.Bitangent = input.Bitangent;
    
    // 입력된 uv텍스처를 출력에 반영한다.
    output.Tex = input.Tex;

    return output;
}