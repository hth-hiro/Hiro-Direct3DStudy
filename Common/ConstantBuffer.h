#pragma once
#include <vector>
#include <d3d11_1.h>
#include <dxgi1_3.h>
#include <d3dcompiler.h>
#include <Directxtk/DDSTextureLoader.h>
#include <directxtk/WICTextureLoader.h>
#include <directxtk/SimpleMath.h>

using namespace DirectX::SimpleMath;

// 상수 버퍼
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

    int UseLighting; // 1 = 빛 계산, 0 = 무시
    int UseTexture;
    Vector2 pad_Boolean;

    int hasTexture;

    int hasNormalMap;
    int hasSpecularMap;
    int hasEmissiveMap;

    Vector4 solidColor;

    int hasMetallicMap;
    int hasRoughnessMap;
    UINT IsRigid;
    UINT RefBoneIndex;

    Matrix ShadowView;
    Matrix ShadowProjection;

    // PBR
    float roughness;
    float metallic;
    float ao;           //
    float gamma;

    Vector4 albedo;
};