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
    
    // Wordl Space로 변환하기 위한 TBN 행렬
    float3x3 TBN = { input.Tangent, input.Bitangent, input.Norm };
    
    float3 worldNormal = normalize(mul(VTS.xyz, TBN));
    
    float4 specularTex = specularMap.Sample(samLinear, input.Tex);

    float4 finalColor = { 0, 0, 0, 0 };
    
     // 텍스처 샘플링 결과
    float4 surface = txDiffuse.Sample(samLinear, input.Tex);

    // 퐁 계산
    //float3 normalVector = normalize(input.Norm);                          
    float3 normalVector = worldNormal; // 노멀 벡터 (N)
    float3 lightVector = normalize(vLightDir.xyz); // 빛의 방향 (L)
    float3 reflectVector = normalize(reflect(lightVector, normalVector)); // 반사 벡터 (R)
    float3 viewVector = normalize(cameraPos.xyz - input.WorldPos); // 뷰 벡터 (V)
    float3 harfVector = normalize(lightVector + viewVector); // 빛의 방향과 뷰 벡터의 중간
    
    float4 ambient = vAmbientColor.rgba * surface.rgba * vMaterialAmbient.rgba;
    float4 diffuse = vDiffuseColor.rgba * surface.rgba * vMaterialDiffuse.rgba * saturate(dot(normalVector, lightVector));
    //float4 specular = vSpecularColor.rgba * pow(saturate(dot(reflectVector, viewVector)), (float)vShininess);

    // 블린 퐁 계산
    float4 specular = /*specularTex **/ vSpecularColor.rgba * vMaterialSpecular.rgba * pow(saturate(dot(normalVector, harfVector)), (float)vShininess); // Specular는 shininess에 따라 빛의 모여듬 정도가 달라짐(커지면 작고 날카로움)
    
    if (UseLighting.x > 0.5f)
    {
        // 기존 라이팅 계산
        // 빛의 계산은 ambient + diffuse + specular 로 구성
        finalColor = saturate(ambient + diffuse + specular); // 빛의 세기는 1을 넘기지 말아야 함.
        // 최종 Alpha : 텍스처의 Alpha 사용 -> 투명 재질 처리용
        finalColor.a = surface.a;
    }
    else
    {
        // UseLighting == 0이면 color 그대로 출력 (Unlit)
        finalColor = surface;
    }
    
    return finalColor;
}