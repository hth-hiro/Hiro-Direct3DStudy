#pragma once
#include <vector>
#include <d3d11_1.h>
#include "Mesh.h"
#include "Bone.h"
#include "../Common/Transform.h"
#include "Model.h"
#include "SkeletalMesh.h"

struct Texture;
using namespace DirectX;

class SkeletalModel : public Model
{
public:
    SkeletalMesh* skeletalMesh = nullptr;
    
    SkeletalModel() = default;
    SkeletalModel(SkeletalMesh* mesh) : skeletalMesh(mesh) {}

    void Draw(
        ID3D11DeviceContext* devcon,
        ID3D11Buffer* cb,
        const XMMATRIX& view,
        const XMMATRIX& proj,

        const Vector4& lightDir,
        const Vector4& ambient,
        const Vector4& diffuse,
        const Vector4& specular,

        const Vector4& shininess,

        const Vector4& cameraPos,
        const bool& useLighting,

        BoneMatrixContainer* pBones = nullptr,
        ID3D11Buffer* boneCB = nullptr,
        int refBoneIndex = -1
    )
    {
        for (size_t i = 0; i < meshes_.size(); ++i)
        {
            if (i < skeletalMesh->m_Sections.size())
                refBoneIndex = skeletalMesh->m_Sections[i].m_RefBoneIndex;

            ConstantBuffer cbObj{};
            cbObj.mWorld = XMMatrixTranspose(transform.GetMatrix());
            cbObj.mView = XMMatrixTranspose(view);
            cbObj.mProjection = XMMatrixTranspose(proj);

            cbObj.vLightDir = lightDir;
            cbObj.vAmbientColor = ambient;
            cbObj.vDiffuseColor = diffuse;
            cbObj.vSpecularColor = specular;

            cbObj.vMaterialAmbient = material.ambient;
            cbObj.vMaterialDiffuse = material.diffuse;
            cbObj.vMaterialSpecular = material.specular;
            cbObj.vShininess = material.shininess;

            cbObj.cameraPos = cameraPos;
            cbObj.UseLighting = useLighting ? 1 : 0;
            cbObj.RefBoneIndex = refBoneIndex;

            auto& mesh = meshes_[i];
            cbObj.hasTexture = 0;
            for (auto& tex : mesh.textures_)
            {
                if (tex.hasTexture) cbObj.hasTexture = 1;
                else cbObj.solidColor = tex.solidColor;

                if (tex.hasNormalMap) cbObj.hasNormalMap = 1;
                if (tex.hasSpecularMap) cbObj.hasSpecularMap = 1;
                if (tex.hasEmissiveMap) cbObj.hasEmissiveMap = 1;
            }

            devcon->UpdateSubresource(cb, 0, nullptr, &cbObj, 0, 0);
            devcon->VSSetConstantBuffers(0, 1, &cb);
            devcon->PSSetConstantBuffers(0, 1, &cb);

            devcon->VSSetConstantBuffers(3, 1, &skeletalMesh->m_pBonePoseBuffer);
            devcon->VSSetConstantBuffers(4, 1, &skeletalMesh->m_pBoneOffsetBuffer);
            

            mesh.Draw(devcon);
        }
    }

};

