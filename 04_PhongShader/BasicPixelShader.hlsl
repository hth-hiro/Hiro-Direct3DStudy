#include "Shared.hlsli"

//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 main(PS_INPUT input) : SV_Target
{
    float4 finalColor = { 0, 0, 0, 0 };
    
     // 텍스처 샘플링 결과
    //float4 surface = float4(255 / 255.0f, 243 / 255.0f, 232 / 255.0f, 1.0f);  // 임의로 색상을 넣은 것
    float4 surface = txDiffuse.Sample(samLinear, input.Tex);

    // 퐁 계산
    float3 normalVector = normalize(input.Norm);                            // 노멀 벡터 (N)
    float3 lightVector = normalize(vLightDir.xyz);                          // 빛의 방향 (L)
    float3 reflectVector = normalize(reflect(lightVector, normalVector));   // 반사 벡터 (R)
    float3 viewVector = normalize(cameraPos.xyz - input.WorldPos);          // 뷰 벡터 (V)
    
    float4 ambient = vAmbientColor.rgba * surface.rgba;                     // Ambient는 방향과 상관 없이 모든 곳에서 일정하게 적용
    float4 diffuse = vDiffuseColor.rgba * surface.rgba * saturate(dot(normalVector, -lightVector)); // Diffuse의 L 방향은 빛이 들어오는 기준이기 때문에 반대(-L)
    float4 specular = vSpecularColor.rgba * pow(saturate(dot(reflectVector, viewVector)), (float)vShininess); // Specular는 shininess에 따라 빛의 모여듬 정도가 달라짐(커지면 작고 날카로움)

    // 벡터와 스칼라 곱의 연산이므로 순서가 중요하지 않음.
    // 최종 빛 : 텍스처 색상 * 빛 색상 * 입사량
    
    // 빛의 계산은 ambient + diffuse + specular 로 구성
    finalColor = saturate(ambient + diffuse + specular);                       // 빛의 세기는 1을 넘기지 말아야 함.

    // 최종 Alpha : 텍스처의 Alpha 사용 -> 투명 재질 처리용
    finalColor.a = vOutputColor.a;
    return finalColor;
}