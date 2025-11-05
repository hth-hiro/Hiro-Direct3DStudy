#pragma once
#include "Model.h"
#include "Bone.h"
#include <assimp\Importer.hpp>
#include <assimp\scene.h>
#include <assimp\postprocess.h>

#include "../Common/Transform.h"

#include "TextureLoader.h"

#include <unordered_map>
#include <map>

using namespace DirectX::SimpleMath;
using namespace DirectX;
using namespace std;

class SkeletalModel;

struct SkeletonInfo
{
	vector<BoneInfo> m_Bones;
	map<string, int> m_BoneMappingTable;	// 이름 -> 인덱스 매핑
	map<string, int> m_MeshMappingTable;	// 이름 -> 인덱스 매핑
	vector<Matrix> m_BoneOffsetMatrices;

    void CreateFromAiScene(const aiScene* pScene);
    BoneInfo* CreateBoneInfo(const aiScene* pScene, const aiNode* pNode);
    BoneInfo* GetBoneInfoByName(const string& name);
    BoneInfo* GetBoneInfoByIndex(int index);

    int GetBoneIndexByMeshName(const string& meshName);
    int GetBoneCount() { return (int)m_Bones.size(); }

	const string& GetBoneName(int index) const			    // 인덱스를 통해 본 이름을 불러온다.
	{
		if (index >= 0 && index < (int)m_Bones.size())
			return m_Bones[index].Name;
		return "";
	}

	int GetBoneIndexByBoneName(const string& boneName) const	// 이름을 통해 본 인덱스를 불러온다.
	{
		auto it = m_BoneMappingTable.find(boneName);
		return it != m_BoneMappingTable.end() ? it->second : -1;
	}

    void CountNode(int& Count, const aiNode* pNode);
};

struct SkeletalMeshSection
{
	vector<BoneWeightVertex> m_BoneWeights;
	vector<Vertex> m_Vertices;
	vector<UINT> m_Indices;
    vector<Texture> m_Textures;
	int m_RefBoneIndex;

	ComPtr<ID3D11Buffer> m_VertexBuffer;
	ComPtr<ID3D11Buffer> m_IndexBuffer;
    ComPtr<ID3D11Buffer> m_BoneWeightBuffer;

	// GPU 함수
	void CreateVertexBuffer(ID3D11Device* dev)
	{
		if (m_Vertices.empty()) return;

		// Create Vertex Buffer
		D3D11_BUFFER_DESC vbd; 

		vbd.Usage = D3D11_USAGE_IMMUTABLE;
		vbd.ByteWidth = static_cast<UINT>(sizeof(Vertex) * m_Vertices.size());
		vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vbd.CPUAccessFlags = 0;
		vbd.MiscFlags = 0;

		D3D11_SUBRESOURCE_DATA initData;
		initData.pSysMem = m_Vertices.data();

		HR_T(dev->CreateBuffer(&vbd, &initData, &m_VertexBuffer));
	}

	void CreateBoneWeightedVertex(ID3D11Device* dev)	// 가중치를 붙인 버텍스
	{
		if (m_BoneWeights.empty()) return;

		D3D11_BUFFER_DESC bd = {};
		bd.Usage = D3D11_USAGE_IMMUTABLE;
		bd.ByteWidth = static_cast<UINT>(sizeof(BoneWeightVertex) * m_BoneWeights.size());
		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER; // 멀티 스트림으로 바인딩할 예정
		bd.CPUAccessFlags = 0;
		bd.MiscFlags = 0;

		D3D11_SUBRESOURCE_DATA initData = {};
		initData.pSysMem = m_BoneWeights.data();

		HR_T(dev->CreateBuffer(&bd, &initData, &m_BoneWeightBuffer));
	}

	void CreateIndexBuffer(ID3D11Device* dev)
	{
		if (m_Indices.empty())  return;

		D3D11_BUFFER_DESC ibd;
		ibd.Usage = D3D11_USAGE_IMMUTABLE;
		ibd.ByteWidth = static_cast<UINT>(sizeof(UINT) * m_Indices.size());
		ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
		ibd.CPUAccessFlags = 0;
		ibd.MiscFlags = 0;

		D3D11_SUBRESOURCE_DATA initData = {};
		initData.pSysMem = m_Indices.data();

		HR_T(dev->CreateBuffer(&ibd, &initData, &m_IndexBuffer));
	}

	void SetSkeletonInfo()
	{
        
	}
};

class SkeletalMesh
{
public:
	vector<SkeletalMeshSection> m_Sections;
	vector<Material> m_Materials;
	vector<Bone> m_Skeleton;
	vector<Animation> m_Animations;
	SkeletonInfo m_SkeletonInfo;
	BoneMatrixContainer m_SkeletonPose;

    ID3D11Buffer* m_pBonePoseBuffer = nullptr;
    ID3D11Buffer* m_pBoneOffsetBuffer = nullptr;

	int m_AnimationsIndex;
	float m_AnimationProcessTime = 0.0f;

    unordered_map<string, shared_ptr<SkeletalMeshSection>> loadedSections;

    Transform transform;

	SkeletalMesh();
	~SkeletalMesh();

	bool Load(ID3D11Device* dev, ID3D11DeviceContext* devcon, const std::string& filePath, const std::string& name);
    void Update(float deltaTime);
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
        const bool& useLighting);

	void Close();

	std::vector<SkeletalModel> models_;

private:
	ID3D11Device* dev_;
	ID3D11DeviceContext* devcon_;
	std::string directory_;

	HWND hwnd_;

	std::unordered_map<std::string, int> m_BoneNameToIndex;

	bool ReadSkeletonMeshFile(ID3D11Device* dev, ID3D11DeviceContext* devcon, const std::string& filePath, const std::string& name); // Load
	void ReadAnimationFile(const aiScene* scene);

	int CreateSkeleton(aiNode* node, int parentBoneIndex = -1);

	void processNode(aiNode* node, const aiScene* scene, SkeletalModel& model);
    Mesh processMesh(aiMesh* mesh, const aiScene* scene, SkeletalModel& model);
    //Bone processMesh(aiMesh* mesh, const aiScene* scene, SkeletalModel& model);
	std::vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName, const aiScene* scene, Model& model);
	ID3D11ShaderResourceView* loadEmbeddedTexture(const aiTexture* embeddedTexture);
};

