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

class Model
{
public:
	std::vector<Mesh> meshes_;
	std::string name;
	Transform transform;

	Model() = default;

	void Draw(ID3D11DeviceContext* devcon)
	{
		for (auto& mesh : meshes_)
		{
			mesh.Draw(devcon);
		}
	}

	void Close()
	{
		for (auto& mesh : meshes_)
		{
			mesh.Close();
		}
		meshes_.clear();
	}
};

class ModelLoader
{
public:
	ModelLoader();
	~ModelLoader();

	//bool Load(ID3D11Device* dev, ID3D11DeviceContext* devcon, std::string filePath);

	bool Load(ID3D11Device* dev, ID3D11DeviceContext* devcon, const std::string& filePath, const std::string& name);
	void Draw(ID3D11DeviceContext* devcon, const std::string& name, int num);
	void Draw(ID3D11DeviceContext* devcon, const std::string& name);

	void Close();
private:
	ID3D11Device* dev_;
	ID3D11DeviceContext* devcon_;
	std::string directory_;
	std::vector<Texture> textures_loaded_;
	HWND hwnd_;

	std::vector<Model> models_;

	void processNode(aiNode* node, const aiScene* scene, Model& model);
	Mesh processMesh(aiMesh* mesh, const aiScene* scene);
	std::vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName, const aiScene* scene);
	ID3D11ShaderResourceView* loadEmbeddedTexture(const aiTexture* embeddedTexture);
};

#endif // !MODEL_LOADER_H