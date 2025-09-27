#pragma once
#include "Model.h"
#include <string>
#include <unordered_map>
#include <memory>
#include <d3d11.h>

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
	bool ParseOBJ(const std::string& filepath, Model* model);	// OBJ ÆÐ¼­
};

