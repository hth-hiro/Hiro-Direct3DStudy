#ifndef MODEL_LOADER_H
#define MODEL_LOADER_H

#include <vector>
#include <d3d11_1.h>
#include <DirectXMath.h>

#include <assimp\Importer.hpp>
#include <assimp\scene.h>
#include <assimp\postprocess.h>

#include "../Common/Transform.h"
#include "Mesh.h"
#include "Model.h"
#include "TextureLoader.h"

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