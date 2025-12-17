#include "StaticMeshSection.h"
#include "../Common/Helper.h"

void StaticMeshSection::Create(ID3D11Device* device)
{
    D3D11_BUFFER_DESC vbd = {};
    vbd.Usage = D3D11_USAGE_DEFAULT;
    vbd.ByteWidth = UINT(sizeof(Vertex) * Vertices.size());
    vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vbd.CPUAccessFlags = 0;

    D3D11_SUBRESOURCE_DATA initVData = {};
    initVData.pSysMem = Vertices.data();

    HRESULT hr = device->CreateBuffer(&vbd, &initVData, VertexBuffer.GetAddressOf());
    if (FAILED(hr))
    {
        MessageBoxA(nullptr, "Vertex Buffer 积己 角菩", "Error", MB_OK);
        return;
    }

    // ======================
    // Index Buffer 积己
    // ======================
    D3D11_BUFFER_DESC ibd = {};
    ibd.Usage = D3D11_USAGE_DEFAULT;
    ibd.ByteWidth = UINT(sizeof(WORD) * Indices.size());
    ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
    ibd.CPUAccessFlags = 0;

    D3D11_SUBRESOURCE_DATA initIData = {};
    initIData.pSysMem = Indices.data();

    hr = device->CreateBuffer(&ibd, &initIData, IndexBuffer.GetAddressOf());
    if (FAILED(hr))
    {
        MessageBoxA(nullptr, "Index Buffer 积己 角菩", "Error", MB_OK);
        return;
    }
}
