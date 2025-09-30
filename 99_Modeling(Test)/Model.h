#pragma once
#include <d3d11.h>
#include <wrl/client.h>
#include <vector>
#include <dxgi1_3.h>
#include <d3dcompiler.h>
#include <Directxtk/DDSTextureLoader.h>
#include <directxtk/WICTextureLoader.h>

#include <directxtk/SimpleMath.h>

#include "ImGuiManager.h"

using namespace DirectX::SimpleMath;

using Microsoft::WRL::ComPtr;

struct Vertex
{
    Vector3 Pos;
    Vector3 Normal;
    Vector2 Tex; 
    Vector3 Tangent;
    Vector3 Bitangent;
};

struct Model
{
    std::vector<Vertex> vertices;
    std::vector<UINT> indices;

    ComPtr<ID3D11Buffer> vertexBuffer;
    ComPtr<ID3D11Buffer> indexBuffer;
    ComPtr<ID3D11Buffer> constantBuffer;
    ComPtr<ID3D11ShaderResourceView> texture;
    ComPtr<ID3D11SamplerState> sampleLinear;
};

struct ConstantBuffer
{
    Matrix mWorld;
    Matrix mView;
    Matrix mProjection;

    Vector4 vLightDir;
    Vector4 vOutputColor;

    Vector4 vAmbientColor;
    Vector4 vDiffuseColor;
    Vector4 vSpecularColor;

    Vector4 vMaterialAmbient;
    Vector4 vMaterialDiffuse;
    Vector4 vMaterialSpecular;

    Vector4 cameraPos;

    Vector4 vShininess;
};