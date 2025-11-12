#include "SkeletalMesh.h"
#include "../Common/Helper.h"
#include "SkeletalModel.h"

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
    Reset();

	if (!ReadSkeletonMeshFile(dev, devcon, filePath, name))
		return false;

    D3D11_BUFFER_DESC boneDesc = {};
    boneDesc.Usage = D3D11_USAGE_DEFAULT;
    boneDesc.ByteWidth = sizeof(BoneBuffer);  // BoneBuffer 구조체 크기
    boneDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    boneDesc.CPUAccessFlags = 0;
    boneDesc.MiscFlags = 0;
    boneDesc.StructureByteStride = 0;

    HR_T(dev->CreateBuffer(&boneDesc, nullptr, &m_pBonePoseBuffer));
    HR_T(dev->CreateBuffer(&boneDesc, nullptr, &m_pBoneOffsetBuffer));

	return true;
}

bool SkeletalMesh::ReadSkeletonMeshFile(ID3D11Device* dev, ID3D11DeviceContext* devcon, const std::string& filePath, const std::string& name)
{
	Assimp::Importer importer;

    importer.SetPropertyBool(AI_CONFIG_IMPORT_FBX_PRESERVE_PIVOTS, false);

	const aiScene* pScene = importer.ReadFile(filePath,
		aiProcess_Triangulate |         // vertex 삼각형 으로 출력
		aiProcess_GenNormals |          // Normal 정보 생성  
		aiProcess_GenUVCoords |         // 텍스처 좌표 생성
		aiProcess_CalcTangentSpace |    // 탄젠트 벡터 생성
        aiProcess_LimitBoneWeights |    // 본의 영향을 받는 정점의 개수 제한
		aiProcess_ConvertToLeftHanded);

	if (pScene == nullptr)
		return false;

	SkeletalModel model(this);
	model.name = name;
	directory_ = filePath.substr(0, filePath.find_last_of("/\\"));

	this->dev_ = dev;
	this->devcon_ = devcon;

    // Bone 생성
    m_SkeletonInfo.CreateFromAiScene(pScene);

    // Section 채우기
	processNode(pScene->mRootNode, pScene, model);

    CreateSkeleton(pScene->mRootNode);

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

			//if (c < m_Skeleton.size())
			//	m_Skeleton[c].m_pBoneAnimation = boneAnim;

            auto it = m_BoneNameToIndex.find(channel->mNodeName.C_Str());
            if (it != m_BoneNameToIndex.end())
                m_Skeleton[it->second].m_pBoneAnimation = boneAnim;
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

    int currentIndex = bone.m_Index;

    for (unsigned int i = 0; i < node->mNumChildren; ++i)
    {
        CreateSkeleton(node->mChildren[i], currentIndex);
    }

	return currentIndex;
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
        Vector3 position, scaling;
		Quaternion rotation;

		if (bone.m_pBoneAnimation != nullptr)
		{
			bone.m_pBoneAnimation->Evaluate(m_AnimationProcessTime, position, rotation, scaling);

			bone.m_Local = Matrix::CreateScale(scaling) * Matrix::CreateFromQuaternion(rotation)
				* Matrix::CreateTranslation(position);
		}

        if (bone.m_ParentIndex == -1)
            position += Vector3(transform.position.x, transform.position.y, transform.position.z);

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

    BoneBuffer bonePoseData{};
    BoneBuffer boneOffsetData{};

    for (int i = 0; i < m_Skeleton.size(); ++i)
    {
        bonePoseData.Bones[i] = m_SkeletonPose.Array[i];
        boneOffsetData.Bones[i] = m_SkeletonInfo.m_Bones[i].m_OffsetMatrix.Transpose();
    }

    devcon_->UpdateSubresource(m_pBonePoseBuffer, 0, nullptr, &bonePoseData, 0, 0);
    devcon_->UpdateSubresource(m_pBoneOffsetBuffer, 0, nullptr, &boneOffsetData, 0, 0);
}

void SkeletalMesh::Draw(
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
    if (m_Skeleton.empty()) return;

    // 본 행렬 업데이트
    
    //devcon->VSSetConstantBuffers(3, 1, &m_pBonePoseBuffer);
    //devcon->VSSetConstantBuffers(4, 1, &m_pBoneOffsetBuffer);

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
            useLighting,
            &m_SkeletonPose,
            m_pBonePoseBuffer
        );
    }
}

void SkeletalMesh::Reset()
{
    m_Skeleton.clear();
    m_SkeletonInfo.m_Bones.clear();
    m_SkeletonPose = {};
    m_Animations.clear();

    for (int i = 0; i < 128; ++i)
    {
        m_SkeletonPose.Array[i] = XMMatrixIdentity();
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

void SkeletalMesh::processNode(aiNode* node, const aiScene* scene, SkeletalModel& model)
{
    if (!node) return;

    for (UINT i = 0; i < node->mNumMeshes; ++i)
    {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        model.meshes_.push_back(this->processMesh(mesh, scene, model));
    }

    for (UINT i = 0; i < node->mNumChildren; ++i)
    {
        this->processNode(node->mChildren[i], scene, model);
    }
}

Mesh SkeletalMesh::processMesh(aiMesh* mesh, const aiScene* scene, SkeletalModel& model)
{
    int boneIndex = m_SkeletonInfo.GetBoneIndexByMeshName(mesh->mName.C_Str());

    SkeletalMeshSection section;
    section.m_RefBoneIndex = boneIndex;
    section.m_Vertices.resize(mesh->mNumVertices);

    // 서로 다른 메시의 버텍스에 대해 루프를 돌린다.
    for (UINT i = 0; i < mesh->mNumVertices; i++)
    {
        section.m_Vertices[i].Pos.x = mesh->mVertices[i].x;
        section.m_Vertices[i].Pos.y = mesh->mVertices[i].y;
        section.m_Vertices[i].Pos.z = mesh->mVertices[i].z;

        if (mesh->HasNormals())
        {
            section.m_Vertices[i].Normal.x = mesh->mNormals[i].x;
            section.m_Vertices[i].Normal.y = mesh->mNormals[i].y;
            section.m_Vertices[i].Normal.z = mesh->mNormals[i].z;
        }
        else {
            section.m_Vertices[i].Normal = {};
        }

        if (mesh->mTextureCoords[0])
        {
            section.m_Vertices[i].Tex.x = (float)mesh->mTextureCoords[0][i].x;
            section.m_Vertices[i].Tex.y = (float)mesh->mTextureCoords[0][i].y;
        }
        else {
            section.m_Vertices[i].Tex = { 0.0f, 0.0f };
        }

        if (mesh->HasTangentsAndBitangents())
        {
            section.m_Vertices[i].Tangent.x = mesh->mTangents[i].x;
            section.m_Vertices[i].Tangent.y = mesh->mTangents[i].y;
            section.m_Vertices[i].Tangent.z = mesh->mTangents[i].z;

            section.m_Vertices[i].Bitangent.x = mesh->mBitangents[i].x;
            section.m_Vertices[i].Bitangent.y = mesh->mBitangents[i].y;
            section.m_Vertices[i].Bitangent.z = mesh->mBitangents[i].z;
        }
        else {
            section.m_Vertices[i].Tangent = { 0.0f, 0.0f, 0.0f };
            section.m_Vertices[i].Bitangent = { 0.0f, 0.0f, 0.0f };
        }
    }

    for (UINT i = 0; i < mesh->mNumFaces; i++)
    {
        aiFace face = mesh->mFaces[i];

        for (UINT j = 0; j < face.mNumIndices; j++)
            section.m_Indices.push_back(face.mIndices[j]);
    }

    if (mesh->mMaterialIndex >= 0)
    {
        aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

        std::vector<Texture> diffuseMaps = this->loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse", scene, model);
        section.m_Textures.insert(section.m_Textures.end(), diffuseMaps.begin(), diffuseMaps.end());

        std::vector<Texture> normalMaps = this->loadMaterialTextures(material, aiTextureType_NORMALS, "texture_normal", scene, model);
        section.m_Textures.insert(section.m_Textures.end(), normalMaps.begin(), normalMaps.end());

        std::vector<Texture> specularMaps = this->loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular", scene, model);
        section.m_Textures.insert(section.m_Textures.end(), specularMaps.begin(), specularMaps.end());

        std::vector<Texture> emissiveMaps = this->loadMaterialTextures(material, aiTextureType_EMISSIVE, "texture_emissive", scene, model);
        section.m_Textures.insert(section.m_Textures.end(), emissiveMaps.begin(), emissiveMaps.end());

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
                section.m_Textures.push_back(colorTex);
            }
        }
    }

    model.textures_loaded_.insert(model.textures_loaded_.end(), section.m_Textures.begin(), section.m_Textures.end());

    section.m_BoneWeights.resize(mesh->mNumVertices);

    for (UINT i = 0; i < mesh->mNumVertices; ++i)
    {
        section.m_BoneWeights[i].Position = Vector3(
            mesh->mVertices[i].x,
            mesh->mVertices[i].y, 
            mesh->mVertices[i].z);

        section.m_BoneWeights[i].Normal = Vector3(
            mesh->mNormals[i].x,
            mesh->mNormals[i].y,
            mesh->mNormals[i].z);

        section.m_BoneWeights[i].TexCoord = Vector2(
            mesh->mTextureCoords[0][i].x,
            mesh->mTextureCoords[0][i].y);

        section.m_BoneWeights[i].Tangent = Vector3(
            mesh->mTangents[i].x,
            mesh->mTangents[i].y,
            mesh->mTangents[i].z);

        section.m_BoneWeights[i].Bitangent = Vector3(
            mesh->mBitangents[i].x,
            mesh->mBitangents[i].y,
            mesh->mBitangents[i].z);
    }

    for (unsigned int i = 0; i < mesh->mNumBones; ++i)
    {
        aiBone* pAiBone = mesh->mBones[i];
        string boneName = pAiBone->mName.C_Str();

        int boneIndex = m_SkeletonInfo.GetBoneIndexByBoneName(boneName);

        aiMatrix4x4 m = pAiBone->mOffsetMatrix;

        m_SkeletonInfo.m_Bones[boneIndex].m_OffsetMatrix = Matrix(
            m.a1, m.b1, m.c1, m.d1,
            m.a2, m.b2, m.c2, m.d2,
            m.a3, m.b3, m.c3, m.d3,
            m.a4, m.b4, m.c4, m.d4
        );

        for (unsigned int w = 0; w < pAiBone->mNumWeights; ++w)
        {
            const aiVertexWeight& weight = pAiBone->mWeights[w];
            UINT vertexId = weight.mVertexId;

            if (vertexId >= section.m_BoneWeights.size()) continue;

            section.m_BoneWeights[vertexId].AddBoneData(boneIndex, weight.mWeight);
        }
    }

    m_Sections.push_back(section);
    int newIndex = static_cast<int>(m_Sections.size() - 1);
    m_SkeletonInfo.m_MeshMappingTable[mesh->mName.C_Str()] = newIndex;

    SkeletalMeshSection& storedSection = m_Sections.back();
    //storedSection.CreateVertexBuffer(dev_);
    storedSection.CreateBoneWeightedVertex(dev_);
    storedSection.CreateIndexBuffer(dev_);
    storedSection.SetSkeletonInfo();



    return Mesh(dev_, storedSection.m_BoneWeights, storedSection.m_Indices, storedSection.m_Textures);
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

void SkeletonInfo::CreateFromAiScene(const aiScene* pScene)
{
    m_Bones.clear();
    m_BoneMappingTable.clear();
    m_MeshMappingTable.clear();

    if (!pScene) return;

    CreateBoneInfo(pScene, pScene->mRootNode);
}

// 본 트리 생성
BoneInfo* SkeletonInfo::CreateBoneInfo(const aiScene* pScene, const aiNode* pNode)
{
    if (!pScene) return nullptr;

    BoneInfo bone(pNode);
    bone.Name = pNode->mName.C_Str();
    bone.Index = static_cast<int>(m_Bones.size());
    bone.ParentIndex = -1;

    if (pNode->mParent)
    {
        string parentName = pNode->mParent->mName.C_Str();
        auto it = m_BoneMappingTable.find(parentName);
        if (it != m_BoneMappingTable.end())
            bone.ParentIndex = it->second;
    }

    m_Bones.push_back(bone);
    m_BoneMappingTable[bone.Name] = bone.Index;

    // 메쉬 이름 매핑 (해당 노드가 메시를 참조하는 경우)
    if (pNode->mNumMeshes > 0 && pNode->mMeshes != nullptr)
    {
        for (unsigned int i = 0; i < pNode->mNumMeshes; ++i)
        {
            unsigned int meshIndex = pNode->mMeshes[i];
            aiMesh* mesh = pScene->mMeshes[meshIndex];

            // 여기서 mesh 이름과 Bone 매핑 처리
            m_MeshMappingTable[mesh->mName.C_Str()] = bone.Index;
        }
    }

    // 재귀적으로 자식들 탐색
    for (unsigned int i = 0; i < pNode->mNumChildren; ++i)
        CreateBoneInfo(pScene, pNode->mChildren[i]);

    return &m_Bones.back();
}

BoneInfo* SkeletonInfo::GetBoneInfoByName(const string& name)
{
    auto it = m_BoneMappingTable.find(name);
    if (it != m_BoneMappingTable.end())
    {
        int index = it->second;
        if (index >= 0 && index < static_cast<int>(m_Bones.size()))
        {
            return &m_Bones[index];
        }
    }

    return nullptr;
}

BoneInfo* SkeletonInfo::GetBoneInfoByIndex(int index)
{
    if (index >= 0 && index < static_cast<int>(m_Bones.size()))
    {
        return &m_Bones[index];
    }

    return nullptr;
}

int SkeletonInfo::GetBoneIndexByMeshName(const string& meshName)
{
    auto it = m_MeshMappingTable.find(meshName);
    if (it != m_MeshMappingTable.end())
        return it->second;

    return -1;
}

void SkeletonInfo::CountNode(int& Count, const aiNode* pNode)
{
    if (!pNode) return;

    Count++;

    for (unsigned int i = 0; i < pNode->mNumChildren; ++i)
    {
        CountNode(Count, pNode->mChildren[i]);
    }
}