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
    
    output.Norm = normalize(mul(input.Norm, (float3x3)World));
    output.Tangent = normalize(mul(input.Tangent, (float3x3)World));
    output.Bitangent = normalize(mul(input.Bitangent, (float3x3) World));
    
    // 입력된 uv텍스처를 출력에 반영한다.
    output.Tex = input.Tex;
    
    // 라이트 시점에서의 화면 좌표 출력을 위함
    output.PositionShadow = mul(float4(output.WorldPos, 1.0f), ShadowView);
    output.PositionShadow = mul(output.PositionShadow, ShadowProjection);
    
    return output;
}