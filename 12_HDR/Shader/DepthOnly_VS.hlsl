#include "../Shader/Shared.hlsli"

PS_INPUT main(VS_INPUT input)
{
    PS_INPUT output = (PS_INPUT) 0;
    float4 pos = input.Pos;
    
    pos = mul(pos, World);
    pos = mul(pos, ShadowView);
    pos = mul(pos, ShadowProjection);
    output.Pos = pos;
	return output;
}