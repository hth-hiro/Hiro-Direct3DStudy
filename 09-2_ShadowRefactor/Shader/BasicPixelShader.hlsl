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
    float3 tangent = normalize(input.Tangent);
    float3 bitangent = normalize(input.Bitangent);
    float3 normal = normalize(input.Norm);
    // Wordl Space로 변환하기 위한 TBN 행렬
    float3x3 TBN = { tangent, bitangent, normal };
    float3 worldNormal = normalize(mul(VTS.xyz, TBN));
    
    float4 txSpecular = hasSpecularMap > 0 ? specularMap.Sample(samLinear, input.Tex) : float4(1, 1, 1, 1);
    float4 txEmissive = hasEmissiveMap > 0 ? emissiveMap.Sample(samLinear, input.Tex) : float4(0, 0, 0, 0);
    
    float3 normalVector = hasNormalMap > 0 ? worldNormal : normal; // 노멀 벡터 (N)      
    //float3 normalVector = - worldNormal;
    float3 lightVector = normalize(vLightDir.xyz);                                    // 빛의 방향 (L)
    float3 reflectVector = normalize(reflect(lightVector, normalVector));             // 반사 벡터 (R)
    float3 viewVector = normalize(cameraPos.xyz - input.WorldPos);                    // 뷰 벡터 (V)
    float3 harfVector = normalize(-lightVector + viewVector);                         // 하프 벡터 (H)
    
    float4 ambient =
    vAmbientColor
    * vMaterialAmbient;
    
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

    if (surface.a < 0.1f)
        discard;
    
    //if (UseLighting > 0)
    //{
    //    finalColor = saturate(ambient + diffuse + specular);
    //    finalColor.a = surface.a;
    //}
    //else
    //{
    //    finalColor = surface;
    //}
    
    //return finalColor;
    
    if (UseLighting == 0)
    {
        return float4(surface.rgb + txEmissive.rgb, surface.a);
    }
    
    
    
    // 그림자 처리
    // Depth를 기록하기 위해서 Shadow의 포지션 값을 정규화
    float currentShadowDepth = input.PositionShadow.z / input.PositionShadow.w;
    float2 uv = input.PositionShadow.xy / input.PositionShadow.w;
    
    uv.y = -uv.y;
    uv = uv * 0.5 + 0.5;    // -1 ~ 1 => 0 ~ 1
    
    // ShadowMap에 기록된 Depth
    
    // 최종 색상 계산 (조명 + 그림자 + 발광)
    
    float shadowFactor = 1.0f;

    if (uv.x >= 0.0 && uv.x <= 1.0 && uv.y >= 0.0 && uv.y <= 1.0)
    {
        float sampleShadowDepth = txShadow.Sample(samLinear, uv).r;
        shadowFactor = (currentShadowDepth > sampleShadowDepth + 0.001) ? 0.0f : 1.0f;
    }

// 조명 계산
    float3 lit = ambient.rgb + diffuse.rgb * shadowFactor + specular.rgb * shadowFactor;
    float4 final = float4(lit + txEmissive.rgb, surface.a);

    return final;
}