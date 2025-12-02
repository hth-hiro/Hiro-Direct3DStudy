#include "../Shader/Shared.hlsli"

PS_INPUT main(VS_INPUT input)
{
    PS_INPUT output = (PS_INPUT) 0;
    float4 pos = input.Pos;
    
    float4x4 matWorld;
    
    matWorld = World;
    
    pos = mul(pos, matWorld);
    pos = mul(pos, ShadowView);
    pos = mul(pos, ShadowProjection);
    output.PositionShadow = pos;
	return output;
}