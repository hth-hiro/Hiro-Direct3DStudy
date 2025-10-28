#include "../Shader/Shared.hlsli"

PS_INPUT main(VS_INPUT_BONED input)
{
    PS_INPUT output = (PS_INPUT) 0;
    
    Matrix boneWorld = mul(gModelMatrices[gRefBoneIndex], World);
    //Matrix boneWorld = mul(gModelMatrices[27], World);
    
    float4 worldPos = mul(input.Pos, boneWorld);
    output.WorldPos = worldPos;

    output.Pos = mul(worldPos, View);
    output.Pos = mul(output.Pos, Projection);
    
    output.Norm = normalize(mul(input.Norm, (float3x3) boneWorld));
    output.Tangent = normalize(mul(input.Tangent, (float3x3) boneWorld));
    output.Bitangent = normalize(mul(input.Bitangent, (float3x3) boneWorld));

    output.Tex = input.Tex;
    
    return output;
}