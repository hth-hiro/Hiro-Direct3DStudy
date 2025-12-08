#include "../Shader/Shared.hlsli"
#define PI 3.14159265359

//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 main(PS_INPUT input) : SV_Target
{
    float4 finalColor = { 0, 0, 0, 0 };
    
    // 텍스처 샘플링
    // 텍스처가 있다면 샘플링, 아니라면 기본 머테리얼 색을 넣는다.
    float4 surface = hasTexture > 0 ? txDiffuse.Sample(samLinear, input.Tex) : solidColor;
    
    if (surface.a < 0.1f)
        discard;

    // 텍스처 적용 여부를 끄면 샘플링을 안하게 만들예정
    
    
    if (UseTexture == 1)
    {
        
    }
    
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
    float3 halfVector = normalize(-lightVector + viewVector); // 하프 벡터 (H)
    
    float4 ambient =
    vAmbientColor
    * vMaterialAmbient;
    
    // 기존 diffuse 방식 (PBR 적용 X)
    //float4 diffuse =
    //vDiffuseColor
    //* vMaterialDiffuse 
    //* surface
    //* saturate(dot(normalVector, -lightVector));
    
    // 블린 퐁 계산
    //float4 specular =
    //vSpecularColor
    //* vMaterialSpecular 
    //* txSpecular.r
    //* pow(saturate(dot(normalVector, halfVector)), (float) vShininess);
    
    // 퐁 계산
    //float4 specular = vSpecularColor.rgba * pow(saturate(dot(reflectVector, viewVector)), (float)vShininess);

    // PBR
    // 법선 분포 함수
    float a = roughness;
    float D = ((a * a) / (PI * pow((pow(dot(normalVector, halfVector), 2) * (a * a - 1) + 1), 2)));
    
    float3 F0 = lerp(float3(0.04f, 0.04f, 0.04f), albedo.rgb, metallic);
    
    
    float3 F = F0 + (1 - F0) * pow((1 - dot(halfVector, viewVector)), 5);
    
    // 폐쇄성 감쇠
    float k = pow((a + 1), 2) / 8;
    float G = dot(normalVector, viewVector) / (dot(normalVector, viewVector) * (1 - k) + k);
    
    float3 kd = lerp(float3(1, 1, 1) - F, float3(0, 0, 0), metallic);
    
    // 최종 반영
    float3 diffuse = kd * surface.rgb / PI * max(dot(normalVector, -lightVector), 0);
    float NdotL = max(dot(normalVector, -lightVector), 0);
    float3 specular = (F * D * G) / max(1e-6f, 4.0f * NdotL * saturate(dot(normalVector, viewVector))) * NdotL;
    
    //float3 specular = (D * F * G) / (4 * saturate(dot(normalVector, -lightVector)) * saturate(dot(normalVector, viewVector)));
    //float3 diffuse = (1.0 - F) * surface.rgb / PI * max(dot(normalVector, -lightVector), 0);

    
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
        shadowFactor = (currentShadowDepth > sampleShadowDepth + 0.0001) ? 0.0f : 1.0f; 
        // 0.001 부분을 늘리면 그림자와 물체가 가까워지고 줄이면 멀어짐
    }

// 조명 계산
    float3 lit = ambient.rgb + diffuse.rgb * shadowFactor + specular.rgb * shadowFactor;
    float4 final = pow(float4(lit + txEmissive.rgb, surface.a), (1 / gamma));

    return final;
}