#include "Shared.hlsli"

//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
PS_INPUT main(VS_INPUT input)
{
    // HLSL������ �� �켱 ����̴�.
    PS_INPUT output = (PS_INPUT) 0;
    output.Pos = mul(input.Pos, World);
    output.Pos = mul(output.Pos, View);
    output.Pos = mul(output.Pos, Projection);

    // output.Norm�� ����������, w = 1�� ����
    // ���� ���ʹ� ���⸸ �ǹ��ϸ�, ��ġ�� �ƴϹǷ� translation�� ����Ǹ� ������ Ʋ����.
    //output.Norm = mul(float4(input.Norm, 1), World).xyz;

    // World����� 4x4 ���������, ���� ���Ϳ��� ��ġ ������ �ʿ����� �����Ƿ� 3x3 ��ķ� ��ȯ�Ѵ�.
    // ���� ���� ���ʹ� ���� �����̹Ƿ� ���̰� 1�̾�� ���� ����� ��Ȯ��.
    // ���� ��� ���� ��Ŀ����� �������� ������� �ʴ´ٸ� ������ �ص� ����������,
    
    //output.Norm = mul(input.Norm, (float3x3) World);
    
    // �������� ����� �Ķ�� ���� ���Ⱑ �������� �ְ��.
    // ���� ����ȭ�� ��ģ��.
    output.Norm = normalize(mul(input.Norm, (float3x3) World));

    return output;
}