#include "Shared.hlsli"

//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float3 ToneMap_Reinhard(float3 hdr)
{
    return hdr / (1.0f + hdr);
}

float4 main(PS_INPUT_TONEMAP input) : SV_Target
{
    float3 hdr = SceneHDR.Sample(LinearSamp, input.Tex).rgb;
    
    // Exposure
    float exposure = max(0.01, Exposure);
    hdr *= exposure;
    
    // ToneMap
    float3 ldr = ToneMap_Reinhard(hdr);
    
    // Gamma
    ldr = pow(saturate(ldr), 1.0f / max(Gamma, E));
    
    
    //return float4(hdr, 1);
    //return float4(0.5, 0.5, 0.5, 1);
    return float4(ldr, 1.0f);
}