#pragma once
#include "../Common/Helper.h"

struct Material
{
    Vector4 ambient = { 0.1f, 0.1f, 0.1f, 0.1f };
    Vector4 diffuse = { 1.0f, 1.0f, 1.0f, 1.0f };
    Vector4 specular = { 1.0f, 1.0f, 1.0f, 1.0f };
    Vector4 shininess = { 1000.0f, 0, 0, 0 };

    std::string diffusePath;
    std::string normalPath;
    std::string specularPath;
    std::string emissivePath;

    ID3D11ShaderResourceView* diffuseSRV = nullptr;
    ID3D11ShaderResourceView* normalSRV = nullptr;
    ID3D11ShaderResourceView* specularSRV = nullptr;
    ID3D11ShaderResourceView* emissiveSRV = nullptr;

    bool hasTexture = false;
    bool hasNormalMap = false;
    bool hasSpecularMap = false;
    bool hasEmissiveMap = false;

    Vector4 solidColor = { 1.0f, 1.0f, 1.0f, 1.0f };    // Èò»ö

    bool useSolidColor = false;

    void Release()
    {
        SAFE_RELEASE(diffuseSRV);
        SAFE_RELEASE(normalSRV);
        SAFE_RELEASE(specularSRV);
        SAFE_RELEASE(emissiveSRV);
    }
};