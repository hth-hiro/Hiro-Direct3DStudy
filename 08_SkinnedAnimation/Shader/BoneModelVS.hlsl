#include "../Shader/Shared.hlsli"

PS_INPUT main(VS_INPUT_BONED input)
{
    PS_INPUT output = (PS_INPUT) 0;
    
    float4 pos = input.Pos;
    
    float4x4 OffsetPose[4];
    OffsetPose[0] = mul(gBoneOffset[input.BlendIndices.x], gBonePose[input.BlendIndices.x]);
    OffsetPose[1] = mul(gBoneOffset[input.BlendIndices.y], gBonePose[input.BlendIndices.y]);
    OffsetPose[2] = mul(gBoneOffset[input.BlendIndices.z], gBonePose[input.BlendIndices.z]);
    OffsetPose[3] = mul(gBoneOffset[input.BlendIndices.w], gBonePose[input.BlendIndices.w]);
    
    float4x4 WeightedOffsetPose;
    WeightedOffsetPose = mul(input.BlendWeights.x, OffsetPose[0]);
    WeightedOffsetPose += mul(input.BlendWeights.y, OffsetPose[1]);
    WeightedOffsetPose += mul(input.BlendWeights.z, OffsetPose[2]);
    WeightedOffsetPose += mul(input.BlendWeights.w, OffsetPose[3]);
    
    float4x4 ModelToWorld = mul(WeightedOffsetPose, World);
    
    pos = mul(pos, ModelToWorld);
    
    output.WorldPos = pos.xyz;

    pos = mul(pos, View);
    pos = mul(pos, Projection);
    
    output.Pos = pos;

    output.Tex = input.Tex;
    output.Norm = normalize(mul(input.Norm, (float3x3) ModelToWorld));
    output.Tangent = normalize(mul(input.Tangent, (float3x3) ModelToWorld));
    //output.Bitangent = normalize(mul(input.Bitangent, (float3x3) ModelToWorld));

    return output;
}