#include "ModelLoader.h"
#include "TextureLoader.h"

ModelLoader::ModelLoader() :
	dev_(nullptr),
	devcon_(nullptr),
	meshes_(),
	directory_(),
	textures_loaded_(),
	hwnd_(nullptr){
}

ModelLoader::~ModelLoader()
{
}

bool ModelLoader::Load(HWND hwnd, ID3D11Device* dev, ID3D11DeviceContext* devcon, std::string filename)
{
	Assimp::Importer importer;

	const aiScene* pScene = importer.ReadFile(filename, aiProcess_Triangulate | aiProcess_ConvertToLeftHanded);

	if (pScene == nullptr)
		return false;

	this->directory_ = filename.substr(0, filename.find_last_of("/\\"));

	this->dev_ = dev;
	this->devcon_ = devcon;
	this->hwnd_ = hwnd;

	processNode(pScene->mRootNode, pScene);

	return true;
}

bool ModelLoader::Load(ID3D11Device* dev, ID3D11DeviceContext* devcon, std::string filename)
{
	Assimp::Importer importer;

	const aiScene* pScene = importer.ReadFile(filename, aiProcess_Triangulate | aiProcess_ConvertToLeftHanded);

	if (pScene == nullptr)
		return false;

	this->directory_ = filename.substr(0, filename.find_last_of("/\\"));

	this->dev_ = dev;
	this->devcon_ = devcon;

	processNode(pScene->mRootNode, pScene);

	return true;
}

void ModelLoader::Draw(ID3D11DeviceContext* devcon)
{
	for (size_t i = 0; i < meshes_.size(); ++i)
	{
		meshes_[i].Draw(devcon);
	}
}

void ModelLoader::Close()
{
	for (auto& t : textures_loaded_)
		t.Release();

	for (size_t i = 0; i < meshes_.size(); ++i)
	{
		meshes_[i].Close();
	}
}

void ModelLoader::processNode(aiNode* node, const aiScene* scene)
{
	for (UINT i = 0; i < node->mNumMeshes; i++)
	{
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		meshes_.push_back(this->processMesh(mesh, scene));
	}

	for (UINT i = 0; i < node->mNumChildren; i++)
	{
		this->processNode(node->mChildren[i], scene);
	}
}

Mesh ModelLoader::processMesh(aiMesh* mesh, const aiScene* scene)
{
	std::vector<VERTEX> vertices;
	std::vector<UINT> indices;
	std::vector<Texture> textures;

	for (UINT i = 0; i < mesh->mNumVertices; i++)
	{
		VERTEX vertex;

		vertex.X = mesh->mVertices[i].x;
		vertex.Y= mesh->mVertices[i].y;
		vertex.Z = mesh->mVertices[i].z;

		if (mesh->mTextureCoords[0])
		{
			vertex.texcoord.x = (float)mesh->mTextureCoords[0][i].x;
			vertex.texcoord.y = (float)mesh->mTextureCoords[0][i].y;
		}

		vertices.push_back(vertex);
	}

	for (UINT i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];

		for (UINT j = 0; j < face.mNumIndices; j++)
		{
			indices.push_back(face.mIndices[j]);
		}
	}

	if (mesh->mMaterialIndex >= 0)
	{
		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

		std::vector<Texture> diffuseMaps = this->loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse", scene);
		textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
	}

	return Mesh(dev_, vertices, indices, textures);
}

std::vector<Texture> ModelLoader::loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName, const aiScene* scene)
{
	std::vector<Texture> textures;
	for (UINT i = 0; i < mat->GetTextureCount(type); i++)
	{
		aiString str;
		mat->GetTexture(type, i, &str);

		bool skip = false;
		for (UINT j = 0; j < textures_loaded_.size(); j++)
		{
			if (std::strcmp(textures_loaded_[j].path.c_str(), str.C_Str()) == 0)
			{
				textures.push_back(textures_loaded_[j]);
				skip = true;
				break;
			}
		}

		if (!skip)
		{
			Texture texture;

			const aiTexture* embeddedTexture = scene->GetEmbeddedTexture(str.C_Str());
			if (embeddedTexture != nullptr)
			{
				texture.texture = loadEmbeddedTexture(embeddedTexture);
			}
			else
			{
				// aiString 경로 무시
				std::string filename = directory_ + '/' + std::string(str.C_Str());
				// Texture 폴더 경로 제거
				size_t pos = filename.find("Textures\\");
				if (pos != std::string::npos)
				{
					filename = filename.substr(pos + 9); // "Textures\" 길이 9
					filename = directory_ + '/' + filename;
				}

				std::wstring filenamews(filename.begin(), filename.end());

				HR_T(TextureLoader::CreateWICTextureFromFile(dev_, devcon_, filenamews.c_str(), nullptr, &texture.texture));
			}

			texture.type = typeName;
			texture.path = str.C_Str();
			textures.push_back(texture);
			this->textures_loaded_.push_back(texture);
		}
	}

	return textures;
}

ID3D11ShaderResourceView* ModelLoader::loadEmbeddedTexture(const aiTexture* embeddedTexture)
{
	ID3D11ShaderResourceView* texture = nullptr;

	if (embeddedTexture->mHeight != 0)
	{
		D3D11_TEXTURE2D_DESC desc;
		desc.Width = embeddedTexture->mWidth;
		desc.Height = embeddedTexture->mHeight;
		desc.MipLevels = 1;
		desc.ArraySize = 1;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
		desc.BindFlags = D3D10_BIND_SHADER_RESOURCE;
		desc.CPUAccessFlags = 0;
		desc.MiscFlags = 0;

		D3D11_SUBRESOURCE_DATA subresourceData;
		subresourceData.pSysMem = embeddedTexture->pcData;
		subresourceData.SysMemPitch = embeddedTexture->mWidth * 4;
		subresourceData.SysMemSlicePitch = embeddedTexture->mWidth * embeddedTexture->mHeight * 4;

		ID3D11Texture2D* texture2D = nullptr;
		HR_T(dev_->CreateTexture2D(&desc, &subresourceData, &texture2D));

		return texture;
	}

	const size_t size = embeddedTexture->mWidth;

	HR_T(TextureLoader::CreateWICTextureFromMemory(dev_, devcon_, reinterpret_cast<const unsigned char*>(embeddedTexture->pcData), size, nullptr, &texture));

	return texture;
}
