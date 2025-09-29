#include "../Shader/Shared.hlsli"

//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 main(PS_INPUT input) : SV_Target
{
    // Texture2D.Sample은 0~1 값 반환
    float3 normalTS = normalMap.Sample(samLinear, input.Tex).xyz;
    
    // 탄젠트 공간에서 정의된 노멀벡터는 -1~ 1값을 가진다.
    float3 VTS = DecodeNormal(normalTS);
    
    float3 worldNormal = normalize(VTS.x * input.Tangent + VTS.y * input.Bitangent + VTS.z * input.Norm);
    
    float4 finalColor = { 0, 0, 0, 0 };
    
     // 텍스처 샘플링 결과
    //float4 surface = float4(255 / 255.0f, 243 / 255.0f, 232 / 255.0f, 1.0f);  // 임의로 색상을 넣은 것
    float4 surface = txDiffuse.Sample(samLinear, input.Tex);

    // 퐁 계산
    //float3 normalVector = normalize(input.Norm);                            // 노멀 벡터 (N)
    float3 normalVector = worldNormal; // 노멀 벡터 (N)
    float3 lightVector = normalize(vLightDir.xyz);                          // 빛의 방향 (L)
    float3 reflectVector = normalize(reflect(lightVector, normalVector));   // 반사 벡터 (R)
    float3 viewVector = normalize(cameraPos.xyz - input.WorldPos);          // 뷰 벡터 (V)
    float3 harfVector = normalize(-lightVector + viewVector);               // 빛의 방향과 뷰 벡터의 중간
    
    float4 ambient = vAmbientColor.rgba * surface.rgba * vMaterialAmbient.rgba;                     // Ambient는 방향과 상관 없이 모든 곳에서 일정하게 적용
    float4 diffuse = vDiffuseColor.rgba * surface.rgba * vMaterialDiffuse.rgba * saturate(dot(normalVector, -lightVector)); // Diffuse의 L 방향은 빛이 들어오는 기준이기 때문에 반대(-L)
    //float4 specular = vSpecularColor.rgba * pow(saturate(dot(reflectVector, viewVector)), (float)vShininess); // Specular는 shininess에 따라 빛의 모여듬 정도가 달라짐(커지면 작고 날카로움)

    // 블린 퐁 계산
    float4 specular = vSpecularColor.rgba * vMaterialSpecular.rgba * pow(saturate(dot(normalVector, harfVector)), (float) vShininess); // Specular는 shininess에 따라 빛의 모여듬 정도가 달라짐(커지면 작고 날카로움)
    
    // 빛의 계산은 ambient + diffuse + specular 로 구성
    finalColor = saturate(ambient + diffuse + specular);                       // 빛의 세기는 1을 넘기지 말아야 함.

    // 최종 Alpha : 텍스처의 Alpha 사용 -> 투명 재질 처리용
    finalColor.a = vOutputColor.a;
    return finalColor;
}