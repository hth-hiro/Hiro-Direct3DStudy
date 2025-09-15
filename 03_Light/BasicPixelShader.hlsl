#include "Shared.fxh"

//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 main(PS_INPUT input) : SV_Target
{
    float4 finalColor = { 0, 0, 0, 0 };
    
    float4 surfaceColor = float4(0, 1, 1, 0);

    float3 N = normalize(input.Norm);       // 노멀 벡터
    float3 L = normalize(vLightDir.xyz);    // 빛의 방향
    
    float diffuse = saturate(dot(N, L));    // 입사량
    
    // 최종 RGB : 빛의 색상 * 표면의 색상 * 입사량 ( N · L )
    // 벡터와 스칼라 곱의 연산이므로 순서가 중요하지 않음.
    finalColor.rgb = vLightColor.rgb * surfaceColor.rgb * diffuse;

    // 최종 Alpha : 텍스처의 Alpha 사용 -> 투명 재질 처리용
    finalColor.a = surfaceColor.a;
    return finalColor;
}