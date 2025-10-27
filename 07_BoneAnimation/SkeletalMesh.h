#pragma once
#include <assimp\Importer.hpp>
#include <assimp\scene.h>
#include <assimp\postprocess.h>

//#include "../Common/Transform.h"
//#include "Mesh.h"
#include "TextureLoader.h"
#include "Model.h"
#include "Animation.h"
#include <unordered_map>
#include <map>

// 리팩토링 목표
// SkeletonInfo는 순수 데이터 저장용
// SkeletonMesh는 실제 애니메이션, GPU 버퍼, 동작 처리 담당
// SkeletalMeshSection은 메시 단위 GPU 처리와 본 가중치(Weight) 버퍼 담당
// 중복 제거

using namespace DirectX::SimpleMath;
using namespace DirectX;
using namespace std;

struct Bone
{
	Matrix m_Local;
	Matrix m_Model;

	int m_ParentIndex;
	int m_Index;
	string m_Name;

	BoneAnimation* m_pBoneAnimation;
};

struct BoneMatrixContainer
{
	Matrix Array[128];
};

struct BoneWeightVertex
{
	int boneIndices[4] = { 0, };
	float weights[4] = { 0.f, };
};

struct BoneInfo
{
	Transform RelativeTransform;
	string ParentBoneName;
	string Name;
};

struct BoneName
{
	string Name;
	bool operator<(const BoneName& other) const { return Name < other.Name; }
	bool operator==(const BoneName& other) const { return Name == other.Name; }
};

struct BoneIndex
{
	int Index;
};

struct MeshName
{
	string Name;
	bool operator<(const MeshName& other) const { return Name < other.Name; }
	bool operator==(const MeshName& other) const { return Name == other.Name; }
};

struct SkeletonInfo
{
	vector<BoneInfo> m_Bones;
	map<BoneName, BoneIndex> m_BoneMappingTable;	// 이름 -> 인덱스 매핑
	map<MeshName, BoneIndex> m_MeshMappingTable;	// 이름 -> 인덱스 매핑
	int m_BoneOffsetMatrices;

	string GetBoneNameByIndex(int index) const			// 인덱스를 통해 본 이름을 불러온다.
	{
		if (index >= 0 && index < (int)m_Bones.size())
			return m_Bones[index].Name;
		return "";
	}

	int GetBoneIndexByName(const string& name) const	// 이름을 통해 본 인덱스를 불러온다.
	{
		auto it = m_BoneMappingTable.find({ name });
		return it != m_BoneMappingTable.end() ? it->second.Index : -1;
	}
};

struct SkeletalMeshSection
{
	vector<BoneWeightVertex> m_BoneWeights;
	vector<Vertex> m_Vertices;
	vector<UINT> m_Indices;
	int m_RefBoneIndex;

	ComPtr<ID3D11Buffer> m_VertexBuffer;
	ComPtr<ID3D11Buffer> m_IndexBuffer;

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

		HR_T(dev->CreateBuffer(&bd, &initData, &m_VertexBuffer));
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

	int m_AnimationsIndex;
	float m_AnimationProcessTime;

	SkeletalMesh();
	~SkeletalMesh();

	bool Load(ID3D11Device* dev, ID3D11DeviceContext* devcon, const std::string& filePath, const std::string& name);
	void Update(float deltaTime);

	void Close();

	std::vector<Model> models_;

private:
	ID3D11Device* dev_;
	ID3D11DeviceContext* devcon_;
	std::string directory_;

	HWND hwnd_;

	std::unordered_map<std::string, int> m_BoneNameToIndex;

	bool ReadSkeletonMeshFile(ID3D11Device* dev, ID3D11DeviceContext* devcon, const std::string& filePath, const std::string& name); // Load
	void ReadAnimationFile(const aiScene* scene);

	int CreateSkeleton(aiNode* node, int parentBoneIndex = -1);

	void processNode(aiNode* node, const aiScene* scene, Model& model, int parentBoneIndex = -1);
	Mesh processMesh(aiMesh* mesh, const aiScene* scene, Model& model);
	std::vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName, const aiScene* scene, Model& model);
	ID3D11ShaderResourceView* loadEmbeddedTexture(const aiTexture* embeddedTexture);
};

