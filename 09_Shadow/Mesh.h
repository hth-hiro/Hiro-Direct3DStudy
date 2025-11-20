#pragma once
#ifndef MESH_H
#define MESH_H

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <stdexcept>
#include <d3d11_1.h>
//#include <DirectXMath.h>

#include <d3d11.h>
#include <wrl/client.h>
#include <dxgi1_3.h>
#include <d3dcompiler.h>
#include <Directxtk/DDSTextureLoader.h>
#include <directxtk/WICTextureLoader.h>

#include <directxtk/SimpleMath.h>

#include "../Common/Helper.h"
#include "Bone.h"

#include "VertexTypes.h"

using namespace DirectX::SimpleMath;

using Microsoft::WRL::ComPtr;

struct Texture {
    std::string type;
    std::string path;
    ID3D11ShaderResourceView* texture;

    bool hasTexture = true;
    bool hasNormalMap = false;
    bool hasSpecularMap = false;
    bool hasEmissiveMap = false;

    Vector4 solidColor = {1.0f, 1.0f, 1.0f, 1.0f};    // »Úªˆ

    void Release() {
        SAFE_RELEASE(texture);
    }
};

class Mesh {
public:
    std::vector<BoneWeightVertex> vertices_;
    std::vector<UINT> indices_;
    std::vector<Texture> textures_;
    ID3D11Device* dev_;

    Mesh(ID3D11Device* dev, const std::vector<BoneWeightVertex>& vertices, const std::vector<UINT>& indices, const std::vector<Texture>& textures) :
        vertices_(vertices),
        indices_(indices),
        textures_(textures),
        dev_(dev),
        VertexBuffer_(nullptr),
        IndexBuffer_(nullptr) {
        this->setupMesh(this->dev_);
    }

    void Draw(ID3D11DeviceContext* devcon) {
        UINT stride = sizeof(BoneWeightVertex);
        UINT offset = 0;

        devcon->IASetVertexBuffers(0, 1, &VertexBuffer_, &stride, &offset);
        devcon->IASetIndexBuffer(IndexBuffer_, DXGI_FORMAT_R32_UINT, 0);

        // 0~4 ΩΩ∑‘ √ ±‚»≠
        ID3D11ShaderResourceView* nullSRV[1] = { nullptr };
        for (int i = 0; i <= 4; i++)
            devcon->PSSetShaderResources(i, 1, nullSRV);

        for (const auto& tex : textures_)
        {
            if (tex.type == "texture_diffuse")
                devcon->PSSetShaderResources(0, 1, &tex.texture);
            else if (tex.type == "texture_normal")
                devcon->PSSetShaderResources(2, 1, &tex.texture);
            else if (tex.type == "texture_specular")
                devcon->PSSetShaderResources(3, 1, &tex.texture);
            else if (tex.type == "texture_emissive")
                devcon->PSSetShaderResources(4, 1, &tex.texture);
        }

        devcon->DrawIndexed(static_cast<UINT>(indices_.size()), 0, 0);
    }

    void Close() {
        SAFE_RELEASE(VertexBuffer_);
        SAFE_RELEASE(IndexBuffer_);
    }
private:
    // Render data
    ID3D11Buffer* VertexBuffer_, * IndexBuffer_;

    // Functions
    // Initializes all the buffer objects/arrays
    void setupMesh(ID3D11Device* dev) {
        HRESULT hr;

        D3D11_BUFFER_DESC vbd;
        vbd.Usage = D3D11_USAGE_DEFAULT;
        vbd.ByteWidth = (sizeof(BoneWeightVertex) * vertices_.size());
        vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        vbd.CPUAccessFlags = 0;
        vbd.MiscFlags = 0;

        D3D11_SUBRESOURCE_DATA initData;
        initData.pSysMem = &vertices_[0];

        hr = dev->CreateBuffer(&vbd, &initData, &VertexBuffer_);
        if (FAILED(hr)) {
            Close();
            throw std::runtime_error("Failed to create vertex buffer.");
        }

        D3D11_BUFFER_DESC ibd;
        ibd.Usage = D3D11_USAGE_IMMUTABLE;
        ibd.ByteWidth = static_cast<UINT>(sizeof(UINT) * indices_.size());
        ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
        ibd.CPUAccessFlags = 0;
        ibd.MiscFlags = 0;

        initData.pSysMem = &indices_[0];

        hr = dev->CreateBuffer(&ibd, &initData, &IndexBuffer_);
        if (FAILED(hr)) {
            Close();
            throw std::runtime_error("Failed to create index buffer.");
        }
    }
};

#endif