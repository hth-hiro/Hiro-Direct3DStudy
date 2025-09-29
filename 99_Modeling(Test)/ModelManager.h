#pragma once
#include "Model.h"
#include "../Common/Helper.h"
#include <memory>
#include <unordered_map>
#include <assimp/Importer.hpp>		// 파일을 읽고 aiScene으로 반환
#include <assimp/scene.h>			// 모델 정보(메시, 머테리얼, 애니메이션) 구조체
#include <assimp/postprocess.h>		// 로드 후 변환/처리 옵션

class ModelManager
{
public:
	ModelManager();
	~ModelManager();
	
	void Initialize(ID3D11Device* device, ID3D11DeviceContext* context);
	bool LoadModel(const std::string& name, const std::string& filepath);
	Model* GetModel(const std::string& name);

	void Release();

private:
	ID3D11Device* m_Device;
	ID3D11DeviceContext* m_Context;
	std::unordered_map<std::string, std::unique_ptr<Model>> m_Models;

	bool CreateBuffers(Model* model);
};