#include "Shared.hlsli"

//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------

// HDR

// SDR -> HDR
// 입력 : Rec.709 기준의 선형 RGB
// 출력 : Rec.2020 기준의 선형 RGB
float3 Rec709ToRec2020(float3 color)
{
    static const float3x3 conversion =
    {
        0.627402, 0.329292, 0.043306,
        0.069095, 0.919544, 0.011360,
        0.016394, 0.088028, 0.895578
    };
    return mul(conversion, color);
}

// PQ 인코딩 - 절대 휘도 기반 곡선
// 입력 : PQ가 기대하는 기준 (10,000 nits)에 대해 정규화된 값
// 출력 : PQ로 인코딩된 비선형 값
float3 LinearToST2084(float3 color)
{
    float m1 = 2610.0 / 4096.0 / 4;
    float m2 = 2523.0 / 4096.0 * 128;
    float c1 = 3424.0 / 4096.0;
    float c2 = 2413.0 / 4096.0 * 32;
    float c3 = 2392.0 / 4096.0 * 32;
    float3 cp = pow(abs(color), m1);
    return pow((c1 + c2 * cp) / (1 + c3 * cp), m2);
}

float4 main(PS_INPUT_TONEMAP input) : SV_Target
{
    // LDR과 동일
    float3 hdr = SceneHDR.Sample(LinearSamp, input.Tex).rgb;
    float3 exposure = hdr * pow(2.0f, Exposure);
    float3 toneMapped = ACESFilm(exposure);
    
    float maxHDRNits = 1000.0f;
    
    const float st2084max = 10000.0f;              // PQ 기준 절대 휘도 (10,000nit)
    const float hdrSclar = maxHDRNits / st2084max;
    float3 Rec2020 = Rec709ToRec2020(toneMapped);
    
    
    float3 final;

    return float4(final, 1.0f);
}