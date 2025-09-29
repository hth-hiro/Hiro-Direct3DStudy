#include "ModelManager.h"

unsigned int importFlags = aiProcess_Triangulate |	// vertex 삼각형으로 출력
aiProcess_GenNormals |	// Normal 정보 생성
aiProcess_GenUVCoords | // 텍스처 좌표 생성
aiProcess_CalcTangentSpace | // 탄젠트 벡터 생성
aiProcess_ConvertToLeftHanded;	// DX용 왼손 좌표계 변환
//aiProcess_PreTransformVertices //노드의 변환 행렬을 적용한 버텍스를 생성한다.(staticMesh 처리용)

ModelManager::ModelManager()
{
	
}

ModelManager::~ModelManager()
{
	Release();
}

void ModelManager::Initialize(ID3D11Device* device, ID3D11DeviceContext* context)
{
	m_Device = device;
	m_Context = context;
}

bool ModelManager::LoadModel(const std::string& name, const std::string& filepath)
{
	Assimp::Importer importer;

	const aiScene* scene = importer.ReadFile(filepath, importFlags);

	if (!scene || !scene->HasMeshes())
	{
		OutputDebugStringA(importer.GetErrorString());
		return false;
	}

	auto model = std::make_unique<Model>();

	// 모든 메시 순회
	for (unsigned int i = 0; i < scene->mNumMeshes; ++i)
	{
		aiMesh* mesh = scene->mMeshes[i];

		// 버텍스 읽기
		for (unsigned int v = 0; v < mesh->mNumVertices; ++v)
		{
			Vertex vertex;
			vertex.Pos = Vector3(
				mesh->mVertices[v].x, mesh->mVertices[v].y, mesh->mVertices[v].z);

			vertex.Normal = Vector3(
				mesh->mNormals[v].x, mesh->mNormals[v].y, mesh->mNormals[v].z);

			if (mesh->mTextureCoords[0])
				vertex.Tex = Vector2(
					mesh->mTextureCoords[0][v].x, mesh->mTextureCoords[0][v].y);
			else vertex.Tex = Vector2(0, 0);

			vertex.Tangent = Vector3(
				mesh->mTangents[v].x, mesh->mTangents[v].y, mesh->mTangents[v].z);

			vertex.Bitangent = Vector3(
				mesh->mBitangents[v].x, mesh->mBitangents[v].y, mesh->mBitangents[v].z);

			model->vertices.push_back(vertex);
		}

		// 인덱스 읽기
		for (unsigned int f = 0; f < mesh->mNumFaces; ++f)
		{
			aiFace face = mesh->mFaces[f];
			for (unsigned int j = 0; j < face.mNumIndices; ++j)
				model->indices.push_back(face.mIndices[j]);
		}
	}

	if (!CreateBuffers(model.get()))
		return false;

	m_Models[name] = std::move(model);

	return true;
}

Model* ModelManager::GetModel(const std::string& name)
{
	auto it = m_Models.find(name);

	if (it != m_Models.end())
		return it->second.get();

	return nullptr;
}

void ModelManager::Release()
{
	if (!m_Models.empty())
		m_Models.clear();

	m_Device = nullptr;
	m_Context = nullptr;
}

bool ModelManager::CreateBuffers(Model* model)
{
	if (!model || model->vertices.empty() || model->indices.empty())
		return false;

	UINT stride = sizeof(Vertex);
	UINT offset = 0;

	// Create Vertex Buffer
	D3D11_BUFFER_DESC vbDesc = {};
	vbDesc.ByteWidth = sizeof(Vertex) * model->vertices.size();
	vbDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbDesc.Usage = D3D11_USAGE_DEFAULT;
	vbDesc.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA vbData = {};
	vbData.pSysMem = model->vertices.data();
	HR_T(m_Device->CreateBuffer(&vbDesc, &vbData, &model->vertexBuffer));

	m_Context->IASetVertexBuffers(0, 1, model->vertexBuffer.GetAddressOf(), &stride, &offset);

	// Create Index Buffer
	vbDesc.ByteWidth = sizeof(UINT) * model->indices.size();
	vbDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	vbDesc.Usage = D3D11_USAGE_DEFAULT;
	vbDesc.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA ibData = {};
	ibData.pSysMem = model->indices.data();

	HR_T(m_Device->CreateBuffer(&vbDesc, &ibData, &model->indexBuffer));

	m_Context->IASetIndexBuffer(model->indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
}