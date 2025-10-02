#include "ModelLoader.h"
#include "../Common/Helper.h"

unsigned int importFlags = aiProcess_Triangulate |    // vertex 삼각형 으로 출력
aiProcess_GenNormals |        // Normal 정보 생성  
aiProcess_GenUVCoords |      // 텍스처 좌표 생성
aiProcess_CalcTangentSpace |  // 탄젠트 벡터 생성
aiProcess_ConvertToLeftHanded;  // DX용 왼손좌표계 변환
//aiProcess_PreTransformVertices   // 노드의 변환행렬을 적용한 버텍스 생성한다.  *StaticMesh로 처리할때만

ModelLoader::ModelLoader() :
	dev_(nullptr),
	devcon_(nullptr),
	meshes_(),
	directory_(),
	textures_loaded_(),
	hwnd_(nullptr)
{
}

ModelLoader::~ModelLoader()
{
}

bool ModelLoader::Load(ID3D11Device* dev, ID3D11DeviceContext* devcon, std::string filePath)
{
	Assimp::Importer importer;

	const aiScene* pScene = importer.ReadFile(filePath, importFlags);

	if (pScene == nullptr)
		return false;

	this->directory_ = filePath.substr(0, filePath.find_last_of("/\\"));

	this->dev_ = dev;
	this->devcon_ = devcon;

	processNode(pScene->mRootNode, pScene);

	return true;
}

bool ModelLoader::Load(std::string filePath)
{
	Assimp::Importer importer;

	const aiScene* pScene = importer.ReadFile(filePath, importFlags);

	if (pScene == nullptr)
		return false;

	this->directory_ = filePath.substr(0, filePath.find_last_of("/\\"));

	processNode(pScene->mRootNode, pScene);
}

void ModelLoader::Draw(ID3D11DeviceContext* devcon) {
	for (size_t i = 0; i < meshes_.size(); ++i)
	{
		// 특정 부분만 렌더를 할 수 있게 가능하다.
	
		//if (i >= 8 && i <= 9) continue;
		//if (!(i == 11)) continue;
		//if (!(i == 12)) continue;

		meshes_[i].Draw(devcon);
	}
}

Mesh ModelLoader::processMesh(aiMesh* mesh, const aiScene* scene)
{
	std::vector<Vertex> vertices;
	std::vector<UINT> indices;
	std::vector<Texture> textures;

	// 서로 다른 메시의 버텍스에 대해 루프를 돌린다.
	for (UINT i = 0; i < mesh->mNumVertices; i++)
	{
		Vertex vertex;

		vertex.Pos.x = mesh->mVertices[i].x;
		vertex.Pos.y = mesh->mVertices[i].y;
		vertex.Pos.z = mesh->mVertices[i].z;

		if (mesh->HasNormals())
		{
			vertex.Normal.x = mesh->mNormals[i].x;
			vertex.Normal.y = mesh->mNormals[i].y;
			vertex.Normal.z = mesh->mNormals[i].z;
		}
		else {
			vertex.Normal = {};
		}

		if (mesh->mTextureCoords[0]) 
		{
			vertex.Tex.x = (float)mesh->mTextureCoords[0][i].x;
			vertex.Tex.y = (float)mesh->mTextureCoords[0][i].y;
		}
		else {
			vertex.Tex = { 0.0f, 0.0f };
		}

		if (mesh->HasTangentsAndBitangents())
		{
			vertex.Tangent.x = mesh->mTangents[i].x;
			vertex.Tangent.y = mesh->mTangents[i].y;
			vertex.Tangent.z = mesh->mTangents[i].z;

			vertex.Bitangent.x = mesh->mBitangents[i].x;
			vertex.Bitangent.y = mesh->mBitangents[i].y;
			vertex.Bitangent.z = mesh->mBitangents[i].z;
		}
		else {
			vertex.Tangent = { 0.0f, 0.0f, 0.0f };
			vertex.Bitangent = { 0.0f, 0.0f, 0.0f };
		}

		vertices.push_back(vertex);
	}

	for (UINT i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];

		for (UINT j = 0; j < face.mNumIndices; j++)
			indices.push_back(face.mIndices[j]);
	}

	if (mesh->mMaterialIndex >= 0)
	{
		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

		std::vector<Texture> diffuseMaps = this->loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse", scene);
		textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
	}

	return Mesh(dev_, vertices, indices, textures);
}

std::vector<Texture> ModelLoader::loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName, const aiScene* scene) {
	std::vector<Texture> textures;
	for (UINT i = 0; i < mat->GetTextureCount(type); i++)
	{
		aiString str;
		mat->GetTexture(type, i, &str);
		// 이전에 이미 같은 텍스처가 로드되었는지 확인, 이미 로드되었다면 새로 로드하지 않고 건너 뜀.
		bool skip = false;

		for (UINT j = 0; j < textures_loaded_.size(); j++)
		{
			if (std::strcmp(textures_loaded_[j].path.c_str(), str.C_Str()) == 0)
			{
				textures.push_back(textures_loaded_[j]);
				skip = true; // 같은 경로를 가진 텍스처가 이미 로드되었으므로 다음 텍스처로 넘어간다. (최적화용)
				break;
			}
		}

		if (!skip) // 만약 텍스처가 아직 로드되지 않았다면 GPU에 업로드
		{   
			Texture texture;

			// 여기서 임베디드 텍스처의 여부를 확인하고,
			// 임베디드가 아니면 경로에서 파일을 로드한다.
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
			this->textures_loaded_.push_back(texture);  // 이 텍스처를 모델 전체에서 로드된 텍스처로 저장하여 불필요하게 중복로드되는 일을 방지한다.
		}
	}
	return textures;
}

void ModelLoader::Close() {
	for (auto& t : textures_loaded_)
		t.Release();

	for (size_t i = 0; i < meshes_.size(); i++)
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

ID3D11ShaderResourceView* ModelLoader::loadEmbeddedTexture(const aiTexture* embeddedTexture)
{
	ID3D11ShaderResourceView* texture = nullptr;

	if (embeddedTexture->mHeight != 0) {
		// 임베디드 텍스처가 압축되지 않은 ARGB8888 형식임을 가정하고 로드
		D3D11_TEXTURE2D_DESC desc;
		desc.Width = embeddedTexture->mWidth;
		desc.Height = embeddedTexture->mHeight;
		desc.MipLevels = 1;
		desc.ArraySize = 1;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		desc.CPUAccessFlags = 0;
		desc.MiscFlags = 0;

		D3D11_SUBRESOURCE_DATA subresourceData;
		subresourceData.pSysMem = embeddedTexture->pcData;
		subresourceData.SysMemPitch = embeddedTexture->mWidth * 4;
		subresourceData.SysMemSlicePitch = embeddedTexture->mWidth * embeddedTexture->mHeight * 4;

		ID3D11Texture2D* texture2D = nullptr;
		HR_T(dev_->CreateTexture2D(&desc, &subresourceData, &texture2D));
		HR_T(dev_->CreateShaderResourceView(texture2D, nullptr, &texture));

		return texture;
	}

	// 임베디드 텍스처가 압축된 형식이라면 mHeight가 0이 됨.
	const size_t size = embeddedTexture->mWidth;

	HR_T(TextureLoader::CreateWICTextureFromMemory(dev_, devcon_, reinterpret_cast<const unsigned char*>(embeddedTexture->pcData), size, nullptr, &texture));

	return texture;
}