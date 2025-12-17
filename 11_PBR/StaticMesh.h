#pragma once
#include <iostream>
#include <vector>
#include <DirectXMath.h>
#include <directxtk/SimpleMath.h>
#include <d3d11_1.h>

#include "StaticMeshSection.h"
#include "Material.h"

using namespace std;
using namespace DirectX;

class StaticMesh
{
public:
    vector<StaticMeshSection> m_StaticMeshSection;
    vector<Material> m_Materials;
    Matrix m_World;

    bool Load(const std::string& filePath, const std::string& name);

    void ReadFile(ID3D11Device* device, const std::string& filePath);

    ~StaticMesh()
    {
        Release();
    }

private:
    ID3D11Device* m_Dev;
    std::string m_Directory;
    std::string m_Name;

    //ID3D11ShaderResourceView* loadEmbeddedTexture(const aiTexture* embeddedTexture);

    void Release()
    {
        for (auto& mat : m_Materials)
        {
            mat.Release();
        }

        m_StaticMeshSection.clear();
        m_Materials.clear();
    }
};

