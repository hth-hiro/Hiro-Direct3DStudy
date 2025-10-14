#ifndef MODEL_LOADER_H
#define MODEL_LOADER_H

#include <vector>
#include <d3d11_1.h>
#include <DirectXMath.h>

#include <assimp\Importer.hpp>
#include <assimp\scene.h>
#include <assimp\postprocess.h>

#include "Mesh.h"
#include "TextureLoader.h"

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

	Vector4 UseLighting; // 1 = 빛 계산, 0 = 무시

	int hasTexture;
	Vector3 padding;

	Vector4 solidColor;
};

struct Transform
{
	XMFLOAT3 position = { 0,0,0 };
	XMFLOAT3 rotation = { 0,0,0 };
	XMFLOAT3 scale	  = { 1,1,1 };

	XMMATRIX GetMatrix() const {
		return XMMatrixScaling(scale.x, scale.y, scale.z) *
			XMMatrixRotationRollPitchYaw(rotation.x, rotation.y, rotation.z) *
			XMMatrixTranslation(position.x, position.y, position.z);
	}
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
	int num = 3;
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
		for (size_t i = 0; i < meshes_.size(); ++i)
		{
			if (name == "ApiMiku")
			{
				if (num == 0)
				{
					if (i >= 8 && i <= 9) continue;
				}
				else if (num == 1)
				{
					if (!(i == 11)) continue;
				}
				else if (num == 2)
				{
					if (!(i == 12)) continue;
				}
			}

			if (name == "SeifukuApiMiku")
			{
				if (num == 19 &&
					(i == 11 || i == 13 || i == 14 ||
						i == 18 || i == 15 || i == 17 ||
						i == 19 || i == 22 || i == 23)
					) continue;

				if (num == 15 &&
					(i == 18 || i == 15 || i == 17 ||
						i == 19 || i == 22 || i == 23)
					) continue;

				if (num == 7 &&
					( i == 19 || i == 22 || i == 23)
					) continue;
			}

			// 상수 버퍼는 매 메시별로 업데이트가 되어야 한다.
			ConstantBuffer cbObj;
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

			cbObj.vOutputColor = XMFLOAT4(1, 1, 1, 1);

			cbObj.cameraPos = cameraPos;

			cbObj.UseLighting = useLighting ? Vector4(1, 0, 0, 0) : Vector4(0, 0, 0, 0);

			const auto& tex = meshes_[i].textures_.empty() ? Texture{} : meshes_[i].textures_[0];

			cbObj.hasTexture = tex.hasTexture ? 1 : 0;
			cbObj.solidColor = tex.solidColor;

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

class ModelLoader
{
public:
	ModelLoader();
	~ModelLoader();

	//bool Load(ID3D11Device* dev, ID3D11DeviceContext* devcon, std::string filePath);

	bool Load(ID3D11Device* dev, ID3D11DeviceContext* devcon, const std::string& filePath, const std::string& name);
	//void Draw(ID3D11DeviceContext* devcon, const std::string& name);

	void Close();

	std::vector<Model> models_;

private:
	ID3D11Device* dev_;
	ID3D11DeviceContext* devcon_;
	std::string directory_;
	
	HWND hwnd_;

	void processNode(aiNode* node, const aiScene* scene, Model& model);
	Mesh processMesh(aiMesh* mesh, const aiScene* scene, Model& model);
	std::vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName, const aiScene* scene, Model& model);
	ID3D11ShaderResourceView* loadEmbeddedTexture(const aiTexture* embeddedTexture);
};

#endif // !MODEL_LOADER_H