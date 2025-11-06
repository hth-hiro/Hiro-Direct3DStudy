#pragma once
#include <iostream>
#include "Animation.h"

#include <assimp\Importer.hpp>
#include <assimp\scene.h>
#include <assimp\postprocess.h>

using namespace std;

struct Bone
{
    Matrix m_Local;
    Matrix m_Model;

    int m_ParentIndex;
    int m_Index;
    string m_Name;

    BoneAnimation* m_pBoneAnimation;
};

struct BoneMatrixContainer
{
    Matrix Array[128];
};

struct BoneBuffer
{
    XMMATRIX Bones[128];
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

struct BoneInfo
{
    string Name;
    int Index;
    int ParentIndex = -1;
    Matrix LocalTransform;
    Matrix GlobalTransform;
    Matrix m_OffsetMatrix;
    
    BoneInfo(const aiNode* pNode)
    {
        Name = string(pNode->mName.C_Str());
        aiMatrix4x4 t = pNode->mTransformation;
        LocalTransform = Matrix(
            (float)t.a1, (float)t.b1, (float)t.c1, (float)t.d1,
            (float)t.a2, (float)t.b2, (float)t.c2, (float)t.d2,
            (float)t.a3, (float)t.b3, (float)t.c3, (float)t.d3,
            (float)t.a4, (float)t.b4, (float)t.c4, (float)t.d4
        );
    }
};

struct BoneName
{
    string Name;
    bool operator<(const BoneName& other) const { return Name < other.Name; }
    bool operator==(const BoneName& other) const { return Name == other.Name; }
};