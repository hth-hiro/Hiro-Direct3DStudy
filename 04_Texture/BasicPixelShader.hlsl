#include "Shared.hlsli"

//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 main(PS_INPUT input) : SV_Target
{
    float4 finalColor = { 0, 0, 0, 0 };
    
     // �ؽ�ó ���ø� ���
    //float4 surfaceColor = float4(255 / 255.0f, 243 / 255.0f, 232 / 255.0f, 1.0f);
    float4 surfaceColor = txDiffuse.Sample(samLinear, input.Tex);
    
    //float3 viewDir = normalize()

    float3 N = normalize(input.Norm);       // ��� ����
    float3 L = normalize(vLightDir.xyz);    // ���� ����
    
    float diffuse = saturate(dot(N, L));    // �Ի緮

    // ���� RGB : ���� ���� * ǥ���� ���� * �Ի緮 ( N �� L )
    // ���Ϳ� ��Į�� ���� �����̹Ƿ� ������ �߿����� ����.
    //finalColor.rgb = vLightColor.rgb * vOutputColor.rgb * diffuse;
    
    // ���� RGB : �ؽ�ó ���� * �� ���� * �Ի緮
    finalColor.rgb = surfaceColor.rgb * vLightColor.rgb * diffuse;

    // ���� Alpha : �ؽ�ó�� Alpha ��� -> ���� ���� ó����
    finalColor.a = vOutputColor.a;
    return finalColor;
}