#include "Shared.hlsli"

//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 main(PS_INPUT input) : SV_Target
{
    float4 finalColor = { 0, 0, 0, 0 };
    
     // 텍스처 샘플링 결과
    //float4 surface = float4(255 / 255.0f, 243 / 255.0f, 232 / 255.0f, 1.0f); 
    float4 surface = txDiffuse.Sample(samLinear, input.Tex);

    float3 N = normalize(input.Norm);       // 노멀 벡터
    float3 L = normalize(vLightDir.xyz);    // 빛의 방향
    float3 R = reflect(-L, N);              // 반사 벡터
    float3 V = normalize(cameraPos - (float3)input.Pos);
    
    float ambient = L;  // 이거 아님
    
    float diffuse = saturate(dot(N, - L));    // 입사량(빛이 들어오는 기준이기 때문에 반대)

    float specular = saturate(dot(R, V));

    float finalLight = ambient + diffuse + specular;
    
    // 벡터와 스칼라 곱의 연산이므로 순서가 중요하지 않음.
    // 최종 RGB : 텍스처 색상 * 빛 색상 * 입사량
    
    // 빛의 계산은 ambient + diffuse + specular 로 구성
  
    finalColor.rgb = surface.rgb * vDiffuseColor.rgb * finalLight;

    // 최종 Alpha : 텍스처의 Alpha 사용 -> 투명 재질 처리용
    finalColor.a = vOutputColor.a;
    return finalColor;
}