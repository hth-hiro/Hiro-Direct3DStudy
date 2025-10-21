#pragma once
#include <vector>
#include <d3d11_1.h>
//#include <DirectXMath.h>

#include "Mesh.h"

#include "../Common/Transform.h"

class Mesh;
struct Texture;

using namespace DirectX;

// 상수 버퍼를 생성한다. 근데 라이트를 곁들인.
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
	Vector3 padding;

	int hasTexture;

	int hasNormalMap;
	int hasSpecularMap;
	int hasEmissiveMap;


	Vector4 solidColor;
};

struct Material
{
	Vector4 ambient = { 0.1f, 0.1f, 0.1f, 0.1f };
	Vector4 diffuse = { 0.0f, 0.0f, 0.0f, 0.0f };
	Vector4 specular = { 0.0f, 0.0f, 0.0f, 0.0f };
	Vector4 shininess = { 1.0f, 0, 0, 0 };
};

class Model
{
public:
	std::vector<Mesh> meshes_;
	std::vector<Texture> textures_loaded_;
	std::string name;
	Transform transform;
	Material material;
	Transform bone;

	Model() = default;

	void Draw(ID3D11DeviceContext* devcon,
		ID3D11Buffer* cb,
		const XMMATRIX& view,
		const XMMATRIX& proj,

		const Vector4& lightDir,
		const Vector4& ambient,
		const Vector4& diffuse,
		const Vector4& specular,

		const Vector4& shininess,

		const Vector4& cameraPos,
		const bool& useLighting
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

	void Close()
	{
		for (auto& mesh : meshes_)
		{
			mesh.Close();
		}
		meshes_.clear();

		for (auto& tex : textures_loaded_)
		{
			tex.Release();
		}
		textures_loaded_.clear();
	}
};