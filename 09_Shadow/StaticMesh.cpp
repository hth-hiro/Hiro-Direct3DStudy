#include "StaticMesh.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <iostream>
#include <directxtk/WICTextureLoader.h>
#include "TextureLoader.h"
#include "../Common/Helper.h"

bool StaticMesh::Load(const std::string& filePath, const std::string& name)
{
    m_Directory = filePath.substr(0, filePath.find_last_of("/\\"));

    //ReadFile(filePath);

    return true;
}

void StaticMesh::ReadFile(ID3D11Device* device, const std::string& filePath)
{
    m_Dev = device;

    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(filePath,
        aiProcess_Triangulate |
        aiProcess_GenNormals |
        aiProcess_GenUVCoords |
        aiProcess_CalcTangentSpace |
        aiProcess_PreTransformVertices |
        aiProcess_ConvertToLeftHanded);

    m_Directory = filePath.substr(0, filePath.find_last_of("/\\"));

    if (!scene || !scene->mRootNode)
    {
        std::cerr << "Failed to load mesh: " << filePath << std::endl;
        return;
    }

    // -------------------------
    // Materials
    // -------------------------
    m_Materials.resize(scene->mNumMaterials);


    auto LoadTexture = [&](aiMaterial* aiMat, aiTextureType type, std::string& path, ID3D11ShaderResourceView** srv, bool& hasFlag)
        {
            aiString texPath;
            if (AI_SUCCESS == aiMat->GetTexture(type, 0, &texPath))
            {
                //const aiTexture* embeddedTexture = scene->GetEmbeddedTexture(texPath.C_Str());
                //if (embeddedTexture != nullptr)
                //{
                //    *srv = loadEmbeddedTexture(embeddedTexture);
                //    hasFlag = true;
                //    path = texPath.C_Str();
                //    return;
                //}

                std::string filename = texPath.C_Str();

                // "Textures\" 제거
                size_t pos = filename.find("Textures\\");
                if (pos != std::string::npos)
                    filename = filename.substr(pos + 9);

                filename = m_Directory + '/' + filename;
                path = filename;

                std::wstring filenamews(filename.begin(), filename.end());
                HRESULT hr = DirectX::CreateWICTextureFromFile(device, filenamews.c_str(), nullptr, srv);
                hasFlag = SUCCEEDED(hr);
                if (!SUCCEEDED(hr))
                    std::cerr << "Failed to load texture: " << filename << std::endl;
            }
        };

    for (unsigned int i = 0; i < scene->mNumMaterials; ++i)
    {
        aiMaterial* aiMat = scene->mMaterials[i];
        Material mat;

        // 색상
        aiColor4D color;
        if (AI_SUCCESS == aiMat->Get(AI_MATKEY_COLOR_DIFFUSE, color))
            mat.diffuse = XMFLOAT4(color.r, color.g, color.b, color.a);

        // 각 텍스처 타입 로드
        LoadTexture(aiMat, aiTextureType_DIFFUSE, mat.diffusePath, &mat.diffuseSRV, mat.hasTexture);
        LoadTexture(aiMat, aiTextureType_NORMALS, mat.normalPath, &mat.normalSRV, mat.hasNormalMap);
        LoadTexture(aiMat, aiTextureType_SPECULAR, mat.specularPath, &mat.specularSRV, mat.hasSpecularMap);
        LoadTexture(aiMat, aiTextureType_EMISSIVE, mat.emissivePath, &mat.emissiveSRV, mat.hasEmissiveMap);

        m_Materials[i] = mat;
    }

    for (unsigned int i = 0; i < scene->mNumMeshes; ++i)
    {
        aiMesh* aiMesh = scene->mMeshes[i];
        StaticMeshSection section;

        section.materialIndex = aiMesh->mMaterialIndex;

        // Vertex
        section.Vertices.resize(aiMesh->mNumVertices);
        for (unsigned int v = 0; v < aiMesh->mNumVertices; ++v)
        {
            Vertex vertex;
            vertex.Pos = XMFLOAT3(aiMesh->mVertices[v].x, aiMesh->mVertices[v].y, aiMesh->mVertices[v].z);
            vertex.Normal = aiMesh->HasNormals() ? XMFLOAT3(aiMesh->mNormals[v].x, aiMesh->mNormals[v].y, aiMesh->mNormals[v].z) : XMFLOAT3(0, 0, 0);
            vertex.Tex = aiMesh->HasTextureCoords(0) ? XMFLOAT2(aiMesh->mTextureCoords[0][v].x, aiMesh->mTextureCoords[0][v].y) : XMFLOAT2(0, 0);
            vertex.Tangent = aiMesh->HasTangentsAndBitangents() ? XMFLOAT3(aiMesh->mTangents[v].x, aiMesh->mTangents[v].y, aiMesh->mTangents[v].z) : XMFLOAT3(0, 0, 0);
            vertex.Bitangent = aiMesh->HasTangentsAndBitangents() ? XMFLOAT3(aiMesh->mBitangents[v].x, aiMesh->mBitangents[v].y, aiMesh->mBitangents[v].z) : XMFLOAT3(0, 0, 0);

            section.Vertices[v] = vertex;
        }

        // Indices
        for (unsigned int f = 0; f < aiMesh->mNumFaces; ++f)
        {
            aiFace& face = aiMesh->mFaces[f];
            for (unsigned int j = 0; j < face.mNumIndices; ++j)
                section.Indices.push_back(static_cast<WORD>(face.mIndices[j]));
        }

        m_StaticMeshSection.push_back(std::move(section));
    }

    //std::function<void(aiNode*)> processNode = [&](aiNode* node)
    //    {
    //        

    //        // 자식 Node 처리
    //        for (unsigned int i = 0; i < node->mNumChildren; ++i)
    //        {
    //            processNode(node->mChildren[i]);
    //        }
    //    };

    //processNode(scene->mRootNode);
}

//ID3D11ShaderResourceView* StaticMesh::loadEmbeddedTexture(const aiTexture* embeddedTexture)
//{
//    ID3D11ShaderResourceView* texture = nullptr;
//
//    if (embeddedTexture->mHeight != 0) {
//        // 임베디드 텍스처가 압축되지 않은 ARGB8888 형식임을 가정하고 로드
//        D3D11_TEXTURE2D_DESC desc;
//        desc.Width = embeddedTexture->mWidth;
//        desc.Height = embeddedTexture->mHeight;
//        desc.MipLevels = 1;
//        desc.ArraySize = 1;
//        desc.SampleDesc.Count = 1;
//        desc.SampleDesc.Quality = 0;
//        desc.Usage = D3D11_USAGE_DEFAULT;
//        desc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
//        desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
//        desc.CPUAccessFlags = 0;
//        desc.MiscFlags = 0;
//
//        D3D11_SUBRESOURCE_DATA subresourceData;
//        subresourceData.pSysMem = embeddedTexture->pcData;
//        subresourceData.SysMemPitch = embeddedTexture->mWidth * 4;
//        subresourceData.SysMemSlicePitch = embeddedTexture->mWidth * embeddedTexture->mHeight * 4;
//
//        ID3D11Texture2D* texture2D = nullptr;
//        HR_T(m_Dev->CreateTexture2D(&desc, &subresourceData, &texture2D));
//        HR_T(m_Dev->CreateShaderResourceView(texture2D, nullptr, &texture));
//
//        return texture;
//    }
//
//    // 임베디드 텍스처가 압축된 형식이라면 mHeight가 0이 됨.
//    const size_t size = embeddedTexture->mWidth;
//
//    HR_T(DirectX::CreateWICTextureFromMemory(m_Dev, reinterpret_cast<const unsigned char*>(embeddedTexture->pcData), size, nullptr, &texture));
//
//    return texture;
//}

