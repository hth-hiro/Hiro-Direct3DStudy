#include "SkeletalMesh.h"
#include "../Common/Helper.h"

SkeletalMesh::SkeletalMesh() :
	dev_(nullptr),
	devcon_(nullptr),
	directory_(),
	hwnd_(nullptr)
{
}

SkeletalMesh::~SkeletalMesh()
{
}

bool SkeletalMesh::Load(ID3D11Device* dev, ID3D11DeviceContext* devcon, const std::string& filePath, const std::string& name)
{
	if (!ReadSkeletonMeshFile(dev, devcon, filePath, name))
		return false;

	for (auto& section : m_Sections)
	{
		section.CreateVertexBuffer(dev);
		section.CreateIndexBuffer(dev);
		section.CreateBoneWeightedVertex(dev);
		section.SetSkeletonInfo();
	}

	return true;
}

bool SkeletalMesh::ReadSkeletonMeshFile(ID3D11Device* dev, ID3D11DeviceContext* devcon, const std::string& filePath, const std::string& name)
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

	ReadAnimationFile(pScene);

	models_.push_back(std::move(model));

	return true;
}

void SkeletalMesh::ReadAnimationFile(const aiScene* scene)
{
	for (unsigned int i = 0; i < scene->mNumAnimations; ++i)
	{
		aiAnimation* aiAnim = scene->mAnimations[i];
		Animation animation;

		animation.Name = aiAnim->mName.C_Str();
		animation.Duration = (float)aiAnim->mDuration / (float)(aiAnim->mTicksPerSecond != 0 ? aiAnim->mTicksPerSecond : 25.0f);
		animation.BoneAnimations.resize(aiAnim->mNumChannels);

		for (unsigned int c = 0; c < aiAnim->mNumChannels; ++c)
		{
			aiNodeAnim* channel = aiAnim->mChannels[c];
			BoneAnimation* boneAnim = new BoneAnimation();

			for (unsigned int k = 0; k < channel->mNumPositionKeys; ++k)
			{
				AnimationKey key{};
				key.Time = static_cast<float>(channel->mPositionKeys[k].mTime / 
					(aiAnim->mTicksPerSecond != 0 ? aiAnim->mTicksPerSecond : 25.0f));

				key.Position = Vector3(channel->mPositionKeys[k].mValue.x,
									channel->mPositionKeys[k].mValue.y,
									channel->mPositionKeys[k].mValue.z);

				boneAnim->AnimationKeys.push_back(key);
			}

			for (unsigned int k = 0; k < channel->mNumRotationKeys; ++k)
			{
				if (k < boneAnim->AnimationKeys.size())
				boneAnim->AnimationKeys[k].Rotation = Quaternion(channel->mRotationKeys[k].mValue.x,
					channel->mRotationKeys[k].mValue.y,
					channel->mRotationKeys[k].mValue.z,
					channel->mRotationKeys[k].mValue.w
				);
			}

			for (unsigned int k = 0; k < channel->mNumScalingKeys; ++k)
			{
				if (k < boneAnim->AnimationKeys.size())
					boneAnim->AnimationKeys[k].Scaling = Vector3(channel->mScalingKeys[k].mValue.x,
						channel->mScalingKeys[k].mValue.y,
						channel->mScalingKeys[k].mValue.z
					);
			}

			if (c < m_Skeleton.size())
				m_Skeleton[c].m_pBoneAnimation = boneAnim;
		}

		m_Animations.push_back(animation);
	}
}

int SkeletalMesh::CreateSkeleton(aiNode* node, int parentBoneIndex)
{
	Bone bone;
	bone.m_Name = node->mName.C_Str();

	// aiMatrix4x4 -> DirectX::SimpleMath::Matrix 변환
	aiMatrix4x4& t = node->mTransformation;
	Matrix mat = Matrix(
		(float)t.a1, (float)t.b1, (float)t.c1, (float)t.d1,
		(float)t.a2, (float)t.b2, (float)t.c2, (float)t.d2,
		(float)t.a3, (float)t.b3, (float)t.c3, (float)t.d3,
		(float)t.a4, (float)t.b4, (float)t.c4, (float)t.d4
	);

	bone.m_Local = mat;
	bone.m_Model = Matrix::Identity;
	bone.m_ParentIndex = parentBoneIndex;
	bone.m_pBoneAnimation = nullptr;
	bone.m_Index = static_cast<int>(m_Skeleton.size());

	m_Skeleton.push_back(bone);
	m_BoneNameToIndex[bone.m_Name] = bone.m_Index;

	int thisIndex = bone.m_Index;
	for (UINT i = 0; i < node->mNumChildren; ++i)
	{
		CreateSkeleton(node->mChildren[i], thisIndex);
	}

	return bone.m_Index;
}

void SkeletalMesh::Update(float deltaTime)
{
	if (!m_Animations.empty())
	{
		m_AnimationProcessTime += deltaTime;
		m_AnimationProcessTime = fmod(m_AnimationProcessTime, m_Animations[m_AnimationsIndex].Duration);
	}

	for (auto& bone : m_Skeleton)
	{
		if (bone.m_pBoneAnimation != nullptr)
		{
			Vector3 position, scaling;
			Quaternion rotation;

			bone.m_pBoneAnimation->Evaluate(m_AnimationProcessTime, position, rotation, scaling);
			bone.m_Local = Matrix::CreateScale(scaling) * Matrix::CreateFromQuaternion(rotation)
				* Matrix::CreateTranslation(position);
		}

		if (bone.m_ParentIndex != -1)
		{
			bone.m_Model = bone.m_Local * m_Skeleton[bone.m_ParentIndex].m_Model;
		}
		else
		{
			bone.m_Model = bone.m_Local;
		}

		m_SkeletonPose.Array[bone.m_Index] = (bone.m_Model).Transpose();
	}
}

void SkeletalMesh::Close()
{
	for (auto& model : models_)
	{
		model.Close();
	}

	for (auto& bone : m_Skeleton)
	{
		if (bone.m_pBoneAnimation)
		{
			delete bone.m_pBoneAnimation;
			bone.m_pBoneAnimation = nullptr;
		}
	}

	models_.clear();
}

void SkeletalMesh::processNode(aiNode* node, const aiScene* scene, Model& model, int parentBoneIndex)
{
	int boneIndex = CreateSkeleton(node, parentBoneIndex);

	for (UINT i = 0; i < node->mNumMeshes; i++)
	{
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		model.meshes_.push_back(this->processMesh(mesh, scene, model));
	}

	for (UINT i = 0; i < node->mNumChildren; i++)
	{
		this->processNode(node->mChildren[i], scene, model, boneIndex);
	}
}

Mesh SkeletalMesh::processMesh(aiMesh* mesh, const aiScene* scene, Model& model)
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
			aiColor4D color(1, 1, 1, 1);

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

	return Mesh(dev_, vertices, indices, textures);
}

std::vector<Texture> SkeletalMesh::loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName, const aiScene* scene, Model& model)
{
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

ID3D11ShaderResourceView* SkeletalMesh::loadEmbeddedTexture(const aiTexture* embeddedTexture)
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