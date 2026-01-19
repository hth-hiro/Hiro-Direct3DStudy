#include "../Shader/Shared.hlsli"

//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 main(PS_INPUT_QUAD input) : SV_Target
{
    float2 uv = input.uv;
    
    float3 baseColor = gGBufferBaseColor.Sample(samplerPoint, uv).rgb;
    float3 normalEnc = gGBufferNormal.Sample(samplerPoint, uv).rgb;
    float3 posWs = gGBufferPosition.Sample(samplerPoint, uv).xyz;
    
    float3 n = DecodeNormal(normalEnc);
    
    float3 dirLightDirWs = normalize(gDirLightDirectionWS.xyz);
    float intensity = gDirLightDirectionWS.w;
    
    float ndotl = saturate(dot(n, -dirLightDirWs));
    
    float3 lightColor = gDirLightColor.rgb;
    
    float3 colorLinear = baseColor * lightColor * ndotl * intensity;

    return float4(colorLinear, 1.0f);
}