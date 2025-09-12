#include "TutorialApp.h"
#include "../Common/Helper.h"
#include <dxgi1_3.h>
#include <directxtk/simplemath.h>
#include <d3dcompiler.h>
#include <wrl/client.h>

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib") // 셰이더 컴파일 시 필요

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

using namespace Microsoft::WRL;
using namespace DirectX::SimpleMath;

struct Vertex
{
    DirectX::XMFLOAT3 position;
    DirectX::XMFLOAT2 uv;
};

TutorialApp::TutorialApp(HINSTANCE hInstance) : GameApp(hInstance)
{

}

TutorialApp::~TutorialApp()
{
    UninitD3D();
    UninitScene();
}

bool TutorialApp::Initialize(UINT Width, UINT Height)
{
    __super::Initialize(Width, Height);

    if (!InitD3D())
        return false;

    if (!InitScene())
        return false;

    return true;
}

void TutorialApp::Update()
{
}

void TutorialApp::Render()
{
    float color[4] = { 0.5f, 0.5f, 0.5f, 1.0f };

    m_pDeviceContext->ClearRenderTargetView(m_pRenderTargetView, color);

    m_pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    m_pDeviceContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &m_VertexBufferStride, &m_VertexBufferOffset);
    m_pDeviceContext->IASetInputLayout(m_pInputLayout);

    m_pDeviceContext->IASetIndexBuffer(m_pIndexBuffer, DXGI_FORMAT_R32_UINT, m_IndexBufferOffset);

    // 셰이더 설정
    m_pDeviceContext->VSSetShader(m_pVertexShader, nullptr, 0);
    m_pDeviceContext->PSSetShader(m_pPixelShader, nullptr, 0);

    //m_pDeviceContext->Draw(m_VertexCount, 0);
    m_pDeviceContext->DrawIndexed(m_IndexCount, 0, 0);

    m_pSwapChain->Present(0, 0);
}

bool TutorialApp::InitD3D()
{
    HRESULT hr = 0;

    //-------1. Create D3D11 Device, DeviceContext-------//

    //-------2. Create DXGI Factory for Swap Chain-------//
    DXGI_SWAP_CHAIN_DESC swapDesc = {};
    swapDesc.BufferCount = 1;
    swapDesc.BufferDesc.Width = m_ClientWidth;
    swapDesc.BufferDesc.Height = m_ClientHeight;
    swapDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    swapDesc.BufferDesc.RefreshRate.Numerator = 60;
    swapDesc.BufferDesc.RefreshRate.Denominator = 1;
    swapDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapDesc.OutputWindow = m_hWnd;
    swapDesc.SampleDesc.Count = 1;
    swapDesc.SampleDesc.Quality = 0;
    swapDesc.Windowed = TRUE;
    swapDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
    swapDesc.Flags = 0;

    hr = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, 0, NULL, NULL,
        D3D11_SDK_VERSION, &swapDesc, &m_pSwapChain, &m_pDevice, NULL, &m_pDeviceContext);
    if (FAILED(hr))
    {
        MessageBox(m_hWnd, L"스왑체인 생성 실패", L"에러", MB_OK);
        return false;
    }

    ID3D11Texture2D* pBackBufferTexture = nullptr;
    hr = m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&pBackBufferTexture);
    if (FAILED(hr)) {
        MessageBox(m_hWnd, L"백버퍼 획득 실패", L"에러", MB_OK);
        return false;
    }

    hr = m_pDevice->CreateRenderTargetView(pBackBufferTexture, NULL, &m_pRenderTargetView);
    if (FAILED(hr)) return false;

    pBackBufferTexture->Release();
    pBackBufferTexture = nullptr;

    m_pDeviceContext->OMSetRenderTargets(1, &m_pRenderTargetView, NULL);

    //-------3. Create RenderTarget View-------//
    D3D11_VIEWPORT viewport = {};
    viewport.TopLeftX = 0;
    viewport.TopLeftY = 0;
    viewport.Width = (float)m_ClientWidth;
    viewport.Height = (float)m_ClientHeight;
    viewport.MinDepth = 0.0f;
    viewport.MaxDepth = 1.0f;
    m_pDeviceContext->RSSetViewports(1, &viewport);

    return true;
}

void TutorialApp::UninitD3D()
{
    m_pRenderTargetView->Release();
    m_pDeviceContext->Release();
    m_pSwapChain->Release();
    m_pDevice->Release();

    m_pRenderTargetView = nullptr;
    m_pDeviceContext = nullptr;
    m_pSwapChain = nullptr;
    m_pDevice = nullptr;
}

bool TutorialApp::InitScene()
{
    HRESULT hr = 0;
    ID3D10Blob* errorMessage = nullptr;

    m_IndexCount = 0;

    std::vector<Vertex> vertices;
    std::vector<UINT> indices;

    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile("../Resource/Hatsune Miku/miku_prefab.fbx", aiProcess_Triangulate | aiProcess_FlipUVs);

    if (!scene || !scene->mRootNode)
    {
        MessageBox(nullptr, L"Assimp: 모델 로드 실패", L"Error", MB_OK | MB_ICONERROR);
        return false;
    }


    // 재귀적으로 메쉬 추출
    std::function<void(aiNode*)> ProcessNode = [&](aiNode* node)
        {
            for (unsigned int i = 0; i < node->mNumMeshes; i++)
            {
                aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];

                for (unsigned int v = 0; v < mesh->mNumVertices; v++)
                {
                    aiVector3D pos = mesh->mVertices[v];
                    Vertex vertex;
                    vertex.position = DirectX::XMFLOAT3(pos.x, pos.y, pos.z);
                    vertices.push_back(vertex);
                }

                for (unsigned int f = 0; f < mesh->mNumFaces; f++)
                {
                    aiFace face = mesh->mFaces[f];
                    for (unsigned int j = 0; j < face.mNumIndices; j++)
                    {
                        indices.push_back(m_IndexCount++);
                    }
                }
            }

            for (unsigned int i = 0; i < node->mNumChildren; i++)
            {
                ProcessNode(node->mChildren[i]);
            }
        };

    ProcessNode(scene->mRootNode);

    // Create Vertex Buffer
    D3D11_BUFFER_DESC vbDesc = {};
    m_VertexCount = vertices.size();
    vbDesc.ByteWidth = sizeof(Vertex) * m_VertexCount;
    vbDesc.CPUAccessFlags = 0;
    vbDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vbDesc.MiscFlags = 0;
    vbDesc.Usage = D3D11_USAGE_DEFAULT;

    D3D11_SUBRESOURCE_DATA vbData = {};
    vbData.pSysMem = vertices.data();
    hr = m_pDevice->CreateBuffer(&vbDesc, &vbData, &m_pVertexBuffer);
    if (FAILED(hr)) return false;

    m_VertexBufferStride = sizeof(Vertex);
    m_VertexBufferOffset = 0;

    // Create Index Buffer
    D3D11_BUFFER_DESC ibDesc = {};
    ibDesc.ByteWidth = sizeof(UINT) * m_IndexCount;
    ibDesc.Usage = D3D11_USAGE_DEFAULT;
    ibDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;

    D3D11_SUBRESOURCE_DATA ibData = {};
    ibData.pSysMem = indices.data();

    hr = m_pDevice->CreateBuffer(&ibDesc, &ibData, &m_pIndexBuffer);
    if (FAILED(hr)) return false;

    m_IndexBufferOffset = 0;

    // Shader Setting
    ID3DBlob* vertexShaderBuffer = nullptr;
    hr = D3DCompileFromFile(L"BasicVertexShader.hlsl", nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE,
        "main",
        "vs_4_0",
        D3DCOMPILE_ENABLE_STRICTNESS, 0, &vertexShaderBuffer, &errorMessage);
    if (FAILED(hr)) return false;

    hr = m_pDevice->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(),
        vertexShaderBuffer->GetBufferSize(), NULL, &m_pVertexShader);
    if (FAILED(hr)) return false;

    D3D11_INPUT_ELEMENT_DESC layout[] =
    {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,0,D3D11_INPUT_PER_VERTEX_DATA,0}
    };


    hr = m_pDevice->CreateInputLayout(layout, ARRAYSIZE(layout), vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), &m_pInputLayout);
    if (FAILED(hr)) return false;

    vertexShaderBuffer->Release();
    vertexShaderBuffer = nullptr;

    ID3DBlob* pixelShaderBuffer = nullptr;

    hr = D3DCompileFromFile(L"BasicPixelShader.hlsl", nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE,
        "main",
        "ps_4_0",
        D3DCOMPILE_ENABLE_STRICTNESS, 0, &pixelShaderBuffer, &errorMessage);
    if (FAILED(hr)) return false;

    hr = m_pDevice->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), NULL, &m_pPixelShader);
    if (FAILED(hr)) return false;

    pixelShaderBuffer->Release();
    pixelShaderBuffer = nullptr;

    return true;
}

void TutorialApp::UninitScene()
{
    m_pVertexBuffer->Release();
    m_pIndexBuffer->Release();
    m_pInputLayout->Release();
    m_pVertexShader->Release();
    m_pPixelShader->Release();

    m_pVertexBuffer = nullptr;
    m_pIndexBuffer = nullptr;
    m_pInputLayout = nullptr;
    m_pVertexShader = nullptr;
    m_pPixelShader = nullptr;
}