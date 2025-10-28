#pragma once
#include <vector>
#include <d3d11_1.h>
#include "Mesh.h"
#include "Bone.h"
#include "../Common/Transform.h"
#include "Model.h"

class Mesh;
struct Texture;
using namespace DirectX;

class SkeletalModel : public Model
{
public:
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
        // 특정 부분만 렌더를 할 수 있게 가능하다.
        ConstantBuffer cbObj;

        for (size_t i = 0; i < meshes_.size(); ++i)
        {
            // 상수 버퍼는 매 메시별로 업데이트가 되어야 한다.
            cbObj.mWorld = XMMatrixTranspose(this->transform.GetMatrix());
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

            cbObj.vOutputColor = XMFLOAT4(1, 1, 1, 1);

            cbObj.cameraPos = cameraPos;

            cbObj.UseLighting = useLighting ? 1 : 0;

            cbObj.RefBoneIndex = refBoneIndex;

            for (auto& tex : meshes_[i].textures_)
            {
                if (tex.hasTexture) cbObj.hasTexture = 1;
                else
                {
                    cbObj.solidColor = tex.solidColor;
                    cbObj.hasTexture = 0;
                }

                if (tex.hasNormalMap) cbObj.hasNormalMap = 1;
                if (tex.hasSpecularMap) cbObj.hasSpecularMap = 1;
                if (tex.hasEmissiveMap) cbObj.hasEmissiveMap = 1;
            }

            devcon->UpdateSubresource(cb, 0, nullptr, &cbObj, 0, 0);
            devcon->VSSetConstantBuffers(0, 1, &cb);
            devcon->PSSetConstantBuffers(0, 1, &cb);

            meshes_[i].Draw(devcon);
        }
    }

};

