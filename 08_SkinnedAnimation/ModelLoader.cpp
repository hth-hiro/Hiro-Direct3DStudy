#include "ModelLoader.h"
#include "../Common/Helper.h"

ModelLoader::ModelLoader() :
	dev_(nullptr),
	devcon_(nullptr),
	directory_(),
	hwnd_(nullptr)
{
}

ModelLoader::~ModelLoader()
{
}

bool ModelLoader::Load(ID3D11Device* dev, ID3D11DeviceContext* devcon, const std::string& filePath, const std::string& name)
{
	Assimp::Importer importer;

	const aiScene* pScene = importer.ReadFile(filePath,
		aiProcess_Triangulate |    // vertex 삼각형 으로 출력
		aiProcess_GenNormals |        // Normal 정보 생성  
		aiProcess_GenUVCoords |      // 텍스처 좌표 생성
		aiProcess_CalcTangentSpace |  // 탄젠트 벡터 생성
		aiProcess_ConvertToLeftHanded);

	if (pScene == nullptr)
		return false;

	Model model;
	model.name = name;
	directory_ = filePath.substr(0, filePath.find_last_of("/\\"));
	
	this->dev_ = dev;
	this->devcon_ = devcon;

	processNode(pScene->mRootNode, pScene, model);

	models_.push_back(std::move(model));

	return true;
}

Mesh ModelLoader::processMesh(aiMesh* mesh, const aiScene* scene, Model& model)
{
	std::vector<BoneWeightVertex> vertices;
	std::vector<UINT> indices;
	std::vector<Texture> textures;

	// 서로 다른 메시의 버텍스에 대해 루프를 돌린다.
	for (UINT i = 0; i < mesh->mNumVertices; i++)
	{
        BoneWeightVertex vertex;

		vertex.Position.x = mesh->mVertices[i].x;
		vertex.Position.y = mesh->mVertices[i].y;
		vertex.Position.z = mesh->mVertices[i].z;

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
			vertex.TexCoord.x = (float)mesh->mTextureCoords[0][i].x;
			vertex.TexCoord.y = (float)mesh->mTextureCoords[0][i].y;
		}
		else {
			vertex.TexCoord = { 0.0f, 0.0f };
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

		std::vector<Texture> diffuseMaps = this->loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse", scene, model);
		textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());

		std::vector<Texture> normalMaps = this->loadMaterialTextures(material, aiTextureType_NORMALS, "texture_normal", scene, model);
		textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());

		std::vector<Texture> specularMaps = this->loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular", scene, model);
		textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());

		std::vector<Texture> emissiveMaps = this->loadMaterialTextures(material, aiTextureType_EMISSIVE, "texture_emissive", scene, model);
		textures.insert(textures.end(), emissiveMaps.begin(), emissiveMaps.end());

		if (diffuseMaps.empty())
		{
			aiColor4D color(1,1,1,1);

			if (AI_SUCCESS == material->Get(AI_MATKEY_BASE_COLOR, color) ||
				AI_SUCCESS == material->Get(AI_MATKEY_COLOR_DIFFUSE, color))
			{
				Texture colorTex{};
				colorTex.type = "baseColor";
				colorTex.solidColor = XMFLOAT4(color.r, color.g, color.b, 1);
				colorTex.hasTexture = false;
				textures.push_back(colorTex);
			}
		}
	}

	model.textures_loaded_.insert(model.textures_loaded_.end(), textures.begin(), textures.end());

	//if (mesh->HasBones())
	//{
	//	for (UINT i = 0; i < mesh->mNumBones; i++)
	//	{
	//		aiBone* bone = mesh->mBones[i];
	//		aiString name = mesh->mBones[i]->mName;
	//	}
	//}

	return Mesh(dev_, vertices, indices, textures);
}

std::vector<Texture> ModelLoader::loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName, const aiScene* scene, Model& model) {
	std::vector<Texture> textures;
	for (UINT i = 0; i < mat->GetTextureCount(type); i++)
	{
		aiString str;
		mat->GetTexture(type, i, &str);
		
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

		if (type == aiTextureType_NORMALS)
			texture.hasNormalMap = true;
		else if (type == aiTextureType_SPECULAR)
			texture.hasSpecularMap = true;
		else if (type == aiTextureType_EMISSIVE)
			texture.hasEmissiveMap = true;

		texture.type = typeName;
		texture.path = str.C_Str();
		textures.push_back(texture);
		//model.textures_loaded_.push_back(texture);  // 이 텍스처를 모델 전체에서 로드된 텍스처로 저장하여 불필요하게 중복로드되는 일을 방지한다.
		
	}
	return textures;
}

void ModelLoader::Draw(
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
    const bool& useLighting)
{
    for (auto& model : models_)
    {
        model.Draw(
            devcon,
            cb,
            view,
            proj,
            lightDir,
            ambient,
            diffuse,
            specular,
            shininess,
            cameraPos,
            useLighting
        );
    }
}

void ModelLoader::Close()
{
	for (auto& model : models_)
	{
		model.Close();
	}
	
	models_.clear();
}

void ModelLoader::processNode(aiNode* node, const aiScene* scene, Model& model)
{
	for (UINT i = 0; i < node->mNumMeshes; i++)
	{
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		model.meshes_.push_back(this->processMesh(mesh, scene, model));
	}

	for (UINT i = 0; i < node->mNumChildren; i++)
	{
		this->processNode(node->mChildren[i], scene, model);
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