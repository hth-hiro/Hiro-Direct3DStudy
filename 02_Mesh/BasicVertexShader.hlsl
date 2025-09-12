#include <shared.fxh>

// ¡§¡° ºŒ¿Ã¥ı.
struct VS_INPUT
{
    float3 Pos : POSITION;
    float4 Color : COLOR;
};

PS_INPUT main(VS_INPUT input)
{
    PS_INPUT output;
    output.Pos = mul(float4(input.Pos, 1.0f), World);
    output.Pos = mul(output.Pos, View);
    output.Pos = mul(output.Pos, Projection);
    output.Color = input.Color;
    return output;
}