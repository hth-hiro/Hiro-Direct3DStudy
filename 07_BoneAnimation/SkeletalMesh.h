#pragma once
#include <assimp\Importer.hpp>
#include <assimp\scene.h>
#include <assimp\postprocess.h>

//#include "../Common/Transform.h"
//#include "Mesh.h"
#include "TextureLoader.h"
#include "Model.h"
#include "Animation.h"

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

class SkeletalMesh
{
public:
	vector<Bone> m_Skeleton;
	BoneMatrixContainer m_SkeletonPose;
	vector<Animation> m_Animations;

	int m_AnimationsIndex;
	float m_AnimationProcessTime;

	SkeletalMesh();
	~SkeletalMesh();

	bool Load(ID3D11Device* dev, ID3D11DeviceContext* devcon, const std::string& filePath, const std::string& name);
	void ReadAnimationFile();
	void Update(float deltaTime);

	void Close();

	std::vector<Model> models_;

private:
	ID3D11Device* dev_;
	ID3D11DeviceContext* devcon_;
	std::string directory_;

	HWND hwnd_;
	bool ReadSkeletonMeshFile(ID3D11Device* dev, ID3D11DeviceContext* devcon, const std::string& filePath, const std::string& name); // Load
	int CreateSkeleton(aiNode* node, int parentBoneIndex = -1);

	void processNode(aiNode* node, const aiScene* scene, Model& model, int parentBoneIndex = -1);
	Mesh processMesh(aiMesh* mesh, const aiScene* scene, Model& model);
	std::vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName, const aiScene* scene, Model& model);
	ID3D11ShaderResourceView* loadEmbeddedTexture(const aiTexture* embeddedTexture);
};

