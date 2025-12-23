#include "Shared.hlsli"

//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------

// LDR

// 선형 공간 값을 비선형으로 바꾸는 단계
float3 LinearToSRGB(float3 linearColor)
{
    return pow(linearColor, 1.0f / 2.2f);
}

float4 main(PS_INPUT_TONEMAP input) : SV_Target
{
    float3 hdr = SceneHDR.Sample(LinearSamp, input.Tex).rgb;
    
    // Exposure
    float exposure = pow(2.0f, Exposure);
    hdr *= exposure;
    
    // ToneMapping
    float3 toneMapped;
    toneMapped = ACESFilm(hdr);
    
    float3 final;
    final = LinearToSRGB(toneMapped);
    


    return float4(final, 1.0f);
    

}