#include "../Shader/Shared.hlsli"

//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 main(PS_INPUT input) : SV_Target
{
    float4 finalColor = { 0, 0, 0, 0 };
    
    // 텍스처 샘플링
    // 텍스처가 있다면 샘플링, 아니라면 기본 머테리얼 색을 넣는다.
    float4 surface = hasTexture > 0 ? txDiffuse.Sample(samLinear, input.Tex) : solidColor;
 
    // Texture2D.Sample은 0~1 값 반환
    float3 txNormal = normalMap.Sample(samLinear, input.Tex).xyz;
    // 탄젠트 공간에서 정의된 노멀벡터는 -1~ 1값을 가진다.
    float3 VTS = DecodeNormal(txNormal);
    // Wordl Space로 변환하기 위한 TBN 행렬
    float3x3 TBN = { input.Tangent, input.Bitangent, input.Norm };
    float3 worldNormal = normalize(mul(VTS.xyz, TBN));
    
    float4 txSpecular = hasSpecularMap > 0 ? specularMap.Sample(samLinear, input.Tex) : float4(1, 1, 1, 1);
    float4 txEmissive = hasEmissiveMap > 0 ? emissiveMap.Sample(samLinear, input.Tex) : float4(0, 0, 0, 0);
    
    float3 normalVector = hasNormalMap > 0 ? worldNormal : normalize(input.Norm); // 노멀 벡터 (N)      
    //float3 normalVector = - worldNormal;
    float3 lightVector = normalize(vLightDir.xyz);                                    // 빛의 방향 (L)
    float3 reflectVector = normalize(reflect(lightVector, normalVector));             // 반사 벡터 (R)
    float3 viewVector = normalize(cameraPos.xyz - input.WorldPos);                    // 뷰 벡터 (V)
    float3 harfVector = normalize(-lightVector + viewVector);                         // 하프 벡터 (H)
    
    float4 ambient =
    vAmbientColor 
    * vMaterialAmbient
    * surface;
    
    float4 diffuse =
    vDiffuseColor
    * vMaterialDiffuse 
    * surface
    * saturate(dot(normalVector, -lightVector));
    
    // 블린 퐁 계산
    float4 specular =
    vSpecularColor
    * vMaterialSpecular 
    * txSpecular.r
    * pow(saturate(dot(normalVector, harfVector)), (float) vShininess);
    
    // 퐁 계산
    //float4 specular = vSpecularColor.rgba * pow(saturate(dot(reflectVector, viewVector)), (float)vShininess);

    if (UseLighting.x > 0.5f)
    {
        finalColor = saturate(ambient + diffuse + specular);
        finalColor.a = surface.a;
    }
    else
    {
        finalColor = surface;
    }
    
    return finalColor + txEmissive;
}