#include <shared.fxh>

// �ȼ� ���̴�.

float4 main(PS_INPUT input) : SV_TARGET
{
    //return float4(0.0f, 1.0f, 0.0f, 1.0f);
    return input.Color;
}