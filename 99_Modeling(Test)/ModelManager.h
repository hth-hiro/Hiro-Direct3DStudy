#pragma once
#include "Model.h"
#include <string>
#include <unordered_map>
#include <memory>
#include <d3d11.h>

//#include <assimp/Importer.hpp>		// 파일을 읽고 aiScene으로 반환
//#include <assimp/scene.h>			// 모델 정보(메시, 머테리얼, 애니메이션) 구조체
//#include <assimp/postprocess.h>		// 로드 후 변환/처리 옵션

//Assimp::Importer importer;
//
//unsigned int importFlags = aiProcess_Triangulate |	// vertex 삼각형으로 출력
//aiProcess_GenNormals |	// Normal 정보 생성
//aiProcess_GenUVCoords | // 텍스처 좌표 생성
//aiProcess_CalcTangentSpace | // 탄젠트 벡터 생성
//aiProcess_ConvertToLeftHanded;	// DX용 왼손 좌표계 변환
////aiProcess_PreTransformVertices //노드의 변환 행렬을 적용한 버텍스를 생성한다.(staticMesh 처리용)
//
//const aiScene* scene = importer.ReadFile("../Resource/", importFlags);

class ModelManager
{
public:
	ModelManager(ID3D11Device* device, ID3D11DeviceContext* context);
	~ModelManager();
	
	bool LoadModel(const std::string& name, const std::string& filepath);
	Model* GetModel(const std::string& name);
	void ReleaseAll();

private:
	ID3D11Device* m_Device;
	ID3D11DeviceContext* m_Context;

	std::unordered_map<std::string, std::unique_ptr<Model>> m_Models;

	bool CreateBuffers(Model* model);
	bool ParseOBJ(const std::string& filepath, Model* model);	// OBJ 패서
};

