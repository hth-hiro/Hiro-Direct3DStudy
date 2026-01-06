#pragma once
#include <DirectXMath.h>
#include <directxtk/SimpleMath.h>
using namespace DirectX;
using namespace DirectX::SimpleMath;

struct Vertex
{
    Vector3 Pos;
    Vector3 Normal;
    Vector2 Tex;
    Vector3 Tangent;
    Vector3 Bitangent;
};

struct BoneWeightVertex
{
    Vector3 Position;
    Vector3 Normal;
    Vector2 TexCoord;
    Vector3 Tangent;
    Vector3 Bitangent;

    // 영향받는 본 수는 최대 4개로 제한
    int boneIndices[4] = { 0, };
    float weights[4] = { 0.f, };

    void AddBoneData(int boneIndex, float weight)
    {
        for (int i = 0; i < 4; i++)
        {
            if (weights[i] == 0.0f)
            {
                boneIndices[i] = boneIndex;
                weights[i] = weight;
                return;
            }
        }
    }
};
