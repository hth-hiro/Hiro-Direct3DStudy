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
    Matrix Array[256];
};

struct BoneBuffer
{
    XMMATRIX Bones[256];
};

struct BoneWeightVertex
{
    int boneIndices[4] = { 0, };
    float weights[4] = { 0.f, };
};

struct BoneInfo
{
    string Name;
    int Index;
    int ParentIndex = -1;
    Matrix LocalTransform;
    Matrix GlobalTransform;
    
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