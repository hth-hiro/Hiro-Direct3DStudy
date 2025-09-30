#include "ModelManager.h"

//unsigned int importFlags = aiProcess_Triangulate |	// vertex 삼각형으로 출력
//aiProcess_GenNormals |	// Normal 정보 생성
//aiProcess_GenUVCoords | // 텍스처 좌표 생성
//aiProcess_CalcTangentSpace | // 탄젠트 벡터 생성
//aiProcess_ConvertToLeftHanded; 
////aiProcess_PreTransformVertices; //노드의 변환 행렬을 적용한 버텍스를 생성한다.(staticMesh 처리용)

unsigned int importFlags =
aiProcess_Triangulate |					// 메시를 삼각형으로 변환
aiProcess_CalcTangentSpace |			// 탄젠트/비탄젠트 계산
aiProcess_GenSmoothNormals |			// 노멀 없으면 부드러운 노멀 생성
aiProcess_ConvertToLeftHanded;			// DX용 왼손 좌표계 변환

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

	/*--------Vertex Shader--------*/
	ID3DBlob* vertexShaderBuffer = nullptr;
	//CompileShaderFromFile(L"BasicVertexShader.hlsl", "main", "vs_4_0", &vertexShaderBuffer);
	CompileShaderFromFile(L"Shader/BasicVertexShader.hlsl", "main", "vs_4_0", &vertexShaderBuffer);

	HR_T(m_Device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(),
		vertexShaderBuffer->GetBufferSize(), NULL, &m_pVertexShader));

	/*--------InputLayout--------*/
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{"BINORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 44, D3D11_INPUT_PER_VERTEX_DATA, 0}
	};

	HR_T(m_Device->CreateInputLayout(layout, ARRAYSIZE(layout), vertexShaderBuffer->GetBufferPointer(),
		vertexShaderBuffer->GetBufferSize(), &m_pInputLayout));

	SAFE_RELEASE(vertexShaderBuffer);

	/*--------Pixel Shader Stage--------*/
	ID3DBlob* pixelShaderBuffer = nullptr;
	CompileShaderFromFile(L"Shader/BasicPixelShader.hlsl", "main", "ps_4_0", &pixelShaderBuffer);

	HR_T(m_Device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(),
		pixelShaderBuffer->GetBufferSize(), NULL, &m_pPixelShader));

	SAFE_RELEASE(pixelShaderBuffer);
}

bool ModelManager::LoadModel(const std::string& name, const std::string& filePath)
{
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(filePath, importFlags);

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
		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

		// 버텍스 읽기
		for (unsigned int v = 0; v < mesh->mNumVertices; ++v)
		{
			Vertex vertex;
			vertex.Pos = Vector3(
				mesh->mVertices[v].x, mesh->mVertices[v].y, mesh->mVertices[v].z);

			vertex.Normal = Vector3(
				mesh->mNormals[v].x, mesh->mNormals[v].y, mesh->mNormals[v].z);

			if (mesh->HasTextureCoords(0))
				vertex.Tex = Vector2(
					mesh->mTextureCoords[0][v].x, mesh->mTextureCoords[0][v].y);
			else vertex.Tex = Vector2(0, 0);

			if (mesh->mTangents)
				vertex.Tangent = Vector3(mesh->mTangents[v].x, mesh->mTangents[v].y, mesh->mTangents[v].z);
			else
				vertex.Tangent = Vector3(0, 0, 0); // 기본값

			if (mesh->mBitangents)
				vertex.Bitangent = Vector3(mesh->mBitangents[v].x, mesh->mBitangents[v].y, mesh->mBitangents[v].z);
			else
				vertex.Bitangent = Vector3(0, 0, 0); // 기본값

			model->vertices.push_back(vertex);
		}

		// 인덱스 읽기
		for (unsigned int f = 0; f < mesh->mNumFaces; ++f)
		{
			aiFace face = mesh->mFaces[f];
			for (unsigned int j = 0; j < face.mNumIndices; ++j)
				model->indices.push_back(face.mIndices[j]);
		}

		if (material->GetTextureCount(aiTextureType_DIFFUSE) > 0)
		{
			aiString path;
			material->GetTexture(aiTextureType_DIFFUSE, 0, &path);

			const aiTexture* embeddedTex = scene->GetEmbeddedTexture(path.C_Str());
			if (embeddedTex)
			{
				aiTexel* data = embeddedTex->pcData;
				size_t size = embeddedTex->mWidth;
			}
			else
			{
				std::string fullPath(path.C_Str());

				// 마지막 슬래시 위치 찾기
				size_t lastSlash = fullPath.find_last_of("/\\");
				std::string relativePath;
				if (lastSlash != std::string::npos)
					relativePath = fullPath.substr(lastSlash + 1);
				else
					relativePath = fullPath; // fallback

				// std::wstring으로 변환
				std::wstring wFilePath(relativePath.begin(), relativePath.end());

				// Resource 폴더 기준으로 최종 경로 생성
				std::wstring finalPath = L"..\\Resource\\" + wFilePath;

				LoadTextureFormFile(finalPath, m_Device, model->texture);
			}

			// 여기에 Sampler State 생성, CreateSamplerState 사용
			D3D11_SAMPLER_DESC sampDesc = {};

			// 샘플링하는 여러가지 옵션을 설정한다.
			sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
			sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
			sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
			sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
			sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
			sampDesc.MinLOD = 0;                                    // 밉맵 최솟값
			sampDesc.MaxLOD = D3D11_FLOAT32_MAX;                    // 밉맵 최대값
			HR_T(m_Device->CreateSamplerState(&sampDesc, model->sampleLinear.GetAddressOf()));
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

void ModelManager::RenderModel(Model* model, const ConstantBuffer& cb)
{
	UINT stride = sizeof(Vertex);
	UINT offset = 0;

	m_Context->IASetVertexBuffers(0, 1, model->vertexBuffer.GetAddressOf(), &stride, &offset);
	m_Context->IASetIndexBuffer(model->indexBuffer.Get(), DXGI_FORMAT_R32_UINT, offset);
	m_Context->PSSetShaderResources(0, 1, model->texture.GetAddressOf());
	m_Context->PSSetSamplers(0, 1, model->sampleLinear.GetAddressOf());
	m_Context->DrawIndexed(model->indices.size(), 0, 1);
}

void ModelManager::RenderModel(const std::string& name, const ConstantBuffer& cb)
{
	auto it = m_Models.find(name);
	if (it == m_Models.end()) return;

	Model* model = it->second.get();

	UINT stride = sizeof(Vertex);
	UINT offset = 0;

	m_Context->IASetVertexBuffers(0, 1, model->vertexBuffer.GetAddressOf(), &stride, &offset);
	m_Context->IASetIndexBuffer(model->indexBuffer.Get(), DXGI_FORMAT_R32_UINT, offset);
	m_Context->IASetInputLayout(m_pInputLayout);

	m_Context->VSSetShader(m_pVertexShader, nullptr, 0);
	m_Context->PSSetShader(m_pPixelShader, nullptr, 0);

	m_Context->VSSetConstantBuffers(0, 1, model->constantBuffer.GetAddressOf());
	m_Context->PSSetConstantBuffers(0, 1, model->constantBuffer.GetAddressOf());
	m_Context->UpdateSubresource(model->constantBuffer.Get(), 0, nullptr, &cb, 0, 0);


	m_Context->PSSetShaderResources(0, 1, model->texture.GetAddressOf());
	m_Context->PSSetSamplers(0, 1, model->sampleLinear.GetAddressOf());
	m_Context->DrawIndexed(model->indices.size(), 0, 1);
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
	HR_T(m_Device->CreateBuffer(&vbDesc, &vbData, model->vertexBuffer.GetAddressOf()));

	// Create Index Buffer
	vbDesc.ByteWidth = sizeof(UINT) * model->indices.size();
	vbDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	vbDesc.Usage = D3D11_USAGE_DEFAULT;
	vbDesc.CPUAccessFlags = 0;
	D3D11_SUBRESOURCE_DATA ibData = {};
	ibData.pSysMem = model->indices.data();
	HR_T(m_Device->CreateBuffer(&vbDesc, &ibData, model->indexBuffer.GetAddressOf()));

	vbDesc.Usage = D3D11_USAGE_DEFAULT;
	vbDesc.ByteWidth = sizeof(ConstantBuffer);
	vbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	vbDesc.CPUAccessFlags = 0;
	HR_T(m_Device->CreateBuffer(&vbDesc, nullptr, model->constantBuffer.GetAddressOf()));
}

void ModelManager::LoadTextureFormFile(const std::wstring& fullPath, ID3D11Device* device, ComPtr<ID3D11ShaderResourceView>& texture)
{
	std::wstring ext = fullPath.substr(fullPath.find_last_of(L".") + 1);
	std::transform(ext.begin(), ext.end(), ext.begin(), ::towlower);

	HRESULT hr = S_OK;

	if (ext == L"dds")
	{
		HR_T(CreateDDSTextureFromFile(device, fullPath.c_str(), nullptr, texture.GetAddressOf()));
	}
	else
	{
		HR_T(CreateWICTextureFromFile(device, fullPath.c_str(), nullptr, texture.GetAddressOf()));
	}
}
