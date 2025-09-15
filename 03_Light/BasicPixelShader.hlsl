#include "Shared.fxh"

//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 main(PS_INPUT input) : SV_Target
{
    float4 finalColor = { 0, 0, 0, 0 };
    
    float4 surfaceColor = float4(0, 1, 1, 0);

    float3 N = normalize(input.Norm);       // ��� ����
    float3 L = normalize(vLightDir.xyz);    // ���� ����
    
    float diffuse = saturate(dot(N, L));    // �Ի緮
    
    // ���� RGB : ���� ���� * ǥ���� ���� * �Ի緮 ( N �� L )
    // ���Ϳ� ��Į�� ���� �����̹Ƿ� ������ �߿����� ����.
    finalColor.rgb = vLightColor.rgb * surfaceColor.rgb * diffuse;

    // ���� Alpha : �ؽ�ó�� Alpha ��� -> ���� ���� ó����
    finalColor.a = surfaceColor.a;
    return finalColor;
}