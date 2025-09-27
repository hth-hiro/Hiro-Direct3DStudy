#include "ModelManager.h"
#include <fstream>
#include <sstream>

ModelManager::ModelManager(ID3D11Device* device, ID3D11DeviceContext* context)
	: m_Device(device), m_Context(context) {
}

ModelManager::~ModelManager()
{
	ReleaseAll();
}

// 여기서 로드한다.
bool ModelManager::LoadModel(const std::string& name, const std::string& filepath)
{
	if (m_Models.find(name) != m_Models.end())
	{
		return true;
	}

	auto model = std::make_unique<Model>();

	if (!ParseOBJ(filepath, model.get())) return false;

	if (!CreateBuffers(model.get())) return false;

	m_Models[name] = std::move(model);

	return true;
}

Model* ModelManager::GetModel(const std::string& name)
{
	return nullptr;
}

void ModelManager::ReleaseAll()
{
}

bool ModelManager::CreateBuffers(Model* model)
{
	return false;
}

bool ModelManager::ParseOBJ(const std::string& filepath, Model* model)
{
	return false;
}
