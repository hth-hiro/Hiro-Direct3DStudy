#include "Shared.hlsli"

//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
PS_INPUT main(VS_INPUT input)
{
    // HLSL에서는 열 우선 행렬이다.
    PS_INPUT output = (PS_INPUT) 0;
    output.Pos = mul(input.Pos, World);
    output.Pos = mul(output.Pos, View);
    output.Pos = mul(output.Pos, Projection);

    // output.Norm을 정의했지만, w = 1로 적용
    // 법선 벡터는 방향만 의미하며, 위치가 아니므로 translation이 적용되면 법선이 틀어짐.
    //output.Norm = mul(float4(input.Norm, 1), World).xyz;

    // World행렬은 4x4 행렬이지만, 법선 벡터에서 위치 정보는 필요하지 않으므로 3x3 행렬로 변환한다.
    // 또한 법선 벡터는 방향 벡터이므로 길이가 1이어야 조명 계산이 정확함.
    // 예를 들어 월드 행렬에서도 스케일이 적용되지 않는다면 다음과 해도 무방하지만,
    
    //output.Norm = mul(input.Norm, (float3x3) World);
    
    // 스케일이 적용된 후라면 빛의 세기가 실제보다 왜곡됨.
    // 따라서 정규화를 거친다.
    output.Norm = normalize(mul(input.Norm, (float3x3) World));

    return output;
}