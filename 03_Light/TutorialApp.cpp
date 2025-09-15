#include "TutorialApp.h"
#include "../Common/Helper.h"
#include <dxgi1_3.h>
#include <d3dcompiler.h>
#include <wrl/client.h>

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib") // 셰이더 컴파일 시 필요

struct Vertex
{
    Vector3 Pos;
    Vector3 Normal;     // 정점에 대한 라이팅 정보
};

// 상수 버퍼를 생성한다. 근데 라이트를 곁들인.
struct ConstantBuffer
{
    Matrix mWorld;
    Matrix mView;
    Matrix mProjection;

    Vector4 vLightDir;
    Vector4 vLightColor;
    Vector4 vOutputColor;
};

TutorialApp::TutorialApp(HINSTANCE hInstance) : GameApp(hInstance)
{

}

TutorialApp::~TutorialApp()
{
    UninitD3D();
    UninitImGUI();
    UninitScene();
}

bool TutorialApp::Initialize(UINT Width, UINT Height)
{
    __super::Initialize(Width, Height);

    if (!InitD3D())
        return false;

    if (!InitImGUI())
        return false;

    if (!InitScene())
        return false;

    return true;
}

void TutorialApp::Update()
{
    m_LightDirsEvaluated = m_InitialLightDirs;

    //XMMATRIX mRotate = XMMatrixRotationY(m_Angle);
    //XMVECTOR vLightDir = XMLoadFloat4(&m_InitialLightDirs);
    //vLightDir = XMVector3Transform(vLightDir, mRotate);
    //XMStoreFloat4(&m_LightDirsEvaluated, vLightDir);

    float PosX = m_GUI.m_PlayerPosX;
    float PosY = m_GUI.m_PlayerPosY;
    float PosZ = m_GUI.m_PlayerPosZ;

    float nearZ = m_GUI.m_NearZ;
    float farZ = m_GUI.m_FarZ;

    XMVECTOR Eye = XMVectorSet(PosX, PosY, PosZ, 0);

    const float minZ = 0.1f;

    XMVECTOR At;

    float finalFarZ = farZ;
    if (farZ - nearZ <= minZ)
    {
        finalFarZ = nearZ + minZ;
    }
    else
    {
        finalFarZ = farZ;
    }

    // 안전하게 좌표 설정
    if (m_GUI.m_FocusParent)
    {
        float dz = m_GUI.m_ObjectPosZ - PosZ;

        if (fabs(dz) < minZ)
        {
            if (dz >= 0) dz = minZ;
            else dz = -minZ;
        }

        At = XMVectorSet(m_GUI.m_ObjectPosX, m_GUI.m_ObjectPosY, PosZ + dz, 0);
    }
    else
    {
        At = XMVectorSet(PosX, PosY, PosZ + finalFarZ, 0);
    }

    XMVECTOR Up = XMVectorSet(0, 1, 0, 0);
    m_World = XMMatrixRotationY(m_Angle) * XMMatrixTranslation(m_GUI.m_ObjectPosX, m_GUI.m_ObjectPosY, m_GUI.m_ObjectPosZ);
    m_Angle += 0.0001f;

    m_View = XMMatrixLookAtLH(Eye, At, Up);
    m_Projection = XMMatrixPerspectiveFovLH(XM_PIDIV2 / m_GUI.m_FOV, m_ClientWidth / (FLOAT)m_ClientHeight, nearZ, finalFarZ);

    //m_InitialLightDirs = { m_GUI.m_LightPosX , m_GUI.m_LightPosY, m_GUI.m_LightPosZ, 1.0f };
}

void TutorialApp::Render()
{
    // Clear
    float color[4] = { 0.5f, 0.5f, 0.5f, 1.0f };
    m_pDeviceContext->OMSetRenderTargets(1, &m_pRenderTargetView, m_pDepthStencilView);
    m_pDeviceContext->ClearRenderTargetView(m_pRenderTargetView, color);
    m_pDeviceContext->ClearDepthStencilView(m_pDepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);

    // Constant Buffer
    ConstantBuffer cb;
    cb.mWorld = XMMatrixTranspose(m_World);
    cb.mView = XMMatrixTranspose(m_View);
    cb.mProjection = XMMatrixTranspose(m_Projection);
    cb.vLightDir = m_LightDirsEvaluated;
    cb.vLightColor = m_LightColor;
    cb.vOutputColor = XMFLOAT4(1, 1, 1, 1); // Parent Obj
    m_pDeviceContext->UpdateSubresource(m_pConstantBuffer, 0, nullptr, &cb, 0, 0);

    // Render the cube
    // Input Assembler
    m_pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    m_pDeviceContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &m_VertexBufferStride, &m_VertexBufferOffset);
    m_pDeviceContext->IASetInputLayout(m_pInputLayout);
    m_pDeviceContext->IASetIndexBuffer(m_pIndexBuffer, DXGI_FORMAT_R16_UINT, 0);
    m_pDeviceContext->VSSetShader(m_pVertexShader, nullptr, 0);
    m_pDeviceContext->VSSetConstantBuffers(0, 1, &m_pConstantBuffer);
    m_pDeviceContext->PSSetShader(m_pPixelShader, nullptr, 0);
    m_pDeviceContext->PSSetConstantBuffers(0, 1, &m_pConstantBuffer);

    m_pDeviceContext->DrawIndexed(m_nIndices, 0, 0);

    // Render the Light
    XMMATRIX mLight = XMMatrixTranslationFromVector(5.0f * XMLoadFloat4(&m_LightDirsEvaluated));
    XMMATRIX mLightScale = XMMatrixScaling(0.2f, 0.2f, 0.2f);
    mLight = mLightScale * mLight;

    cb.mWorld = XMMatrixTranspose(mLight);
    cb.vOutputColor = m_LightColor;
    m_pDeviceContext->UpdateSubresource(m_pConstantBuffer, 0, nullptr, &cb, 0, 0);

    m_pDeviceContext->PSSetShader(m_pPixelShaderSolid, nullptr, 0);
    m_pDeviceContext->DrawIndexed(m_nIndices, 0, 0);

    // GUI는 맨 나중에 렌더하도록 한다.
    m_GUI.Render();

    m_pSwapChain->Present(0, 0);
}

bool TutorialApp::InitD3D()
{
    HRESULT hr = 0;

    //-------Create DXGI Factory for Swap Chain-------//
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
    if (FAILED(hr))
    {
        MessageBox(m_hWnd, L"백버퍼 획득 실패", L"에러", MB_OK);
        return false;
    }

    //-------Create D3D11 Device, DeviceContext-------//
    hr = m_pDevice->CreateRenderTargetView(pBackBufferTexture, NULL, &m_pRenderTargetView);
    if (FAILED(hr))
    {
        MessageBox(m_hWnd, L"렌더 타겟 뷰 생성 실패", L"에러", MB_OK);
        return false;
    }

    SAFE_RELEASE(pBackBufferTexture);

    D3D11_TEXTURE2D_DESC depthDesc = {};
    depthDesc.Width = m_ClientWidth;
    depthDesc.Height = m_ClientHeight;
    depthDesc.MipLevels = 1;
    depthDesc.ArraySize = 1;
    depthDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    depthDesc.SampleDesc.Count = 1;
    depthDesc.Usage = D3D11_USAGE_DEFAULT;
    depthDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;

    ID3D11Texture2D* depthStencilBuffer = nullptr;
    m_pDevice->CreateTexture2D(&depthDesc, nullptr, &depthStencilBuffer);

    D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
    dsvDesc.Format = depthDesc.Format;
    dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;

    m_pDevice->CreateDepthStencilView(depthStencilBuffer, &dsvDesc, &m_pDepthStencilView);

    // 깊이 테스트
    D3D11_DEPTH_STENCIL_DESC dsDesc = {};
    dsDesc.DepthEnable = true;
    dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    dsDesc.DepthFunc = D3D11_COMPARISON_LESS;

    ID3D11DepthStencilState* depthStencilState;
    m_pDevice->CreateDepthStencilState(&dsDesc, &depthStencilState);
    m_pDeviceContext->OMSetDepthStencilState(depthStencilState, 0);

    /*-------Create RenderTarget View-------*/
    D3D11_VIEWPORT viewport = {};
    viewport.TopLeftX = 0;
    viewport.TopLeftY = 0;
    viewport.Width = (float)m_ClientWidth;
    viewport.Height = (float)m_ClientHeight;
    viewport.MinDepth = 0.0f;
    viewport.MaxDepth = 1.0f;
    m_pDeviceContext->RSSetViewports(1, &viewport);
    m_pDeviceContext->OMSetRenderTargets(1, &m_pRenderTargetView, m_pDepthStencilView);

    return true;
}

void TutorialApp::UninitD3D()
{
    SAFE_RELEASE(m_pRenderTargetView);
    SAFE_RELEASE(m_pDeviceContext);
    SAFE_RELEASE(m_pSwapChain);
    SAFE_RELEASE(m_pDevice);
}

bool TutorialApp::InitImGUI()
{
    m_GUI.Initialize(this->m_pDevice, this->m_pDeviceContext);

    return true;
}

void TutorialApp::UninitImGUI()
{
    m_GUI.Release();
}

bool TutorialApp::InitScene()
{
    HRESULT hr = 0;
    ID3D10Blob* errorMessage = nullptr;

    Vertex vertices[] =
    {
        { Vector3(-1.0f, 1.0f, -1.0f),	Vector3(0.0f, 1.0f, 0.0f) },// Normal Y +	 
        { Vector3(1.0f, 1.0f, -1.0f),	Vector3(0.0f, 1.0f, 0.0f) },
        { Vector3(1.0f, 1.0f, 1.0f),	Vector3(0.0f, 1.0f, 0.0f) },
        { Vector3(-1.0f, 1.0f, 1.0f),	Vector3(0.0f, 1.0f, 0.0f) },

        { Vector3(-1.0f, -1.0f, -1.0f), Vector3(0.0f, -1.0f, 0.0f) },// Normal Y -		
        { Vector3(1.0f, -1.0f, -1.0f),	Vector3(0.0f, -1.0f, 0.0f) },
        { Vector3(1.0f, -1.0f, 1.0f),	Vector3(0.0f, -1.0f, 0.0f) },
        { Vector3(-1.0f, -1.0f, 1.0f),	Vector3(0.0f, -1.0f, 0.0f) },

        { Vector3(-1.0f, -1.0f, 1.0f),	Vector3(-1.0f, 0.0f, 0.0f) },//	Normal X -
        { Vector3(-1.0f, -1.0f, -1.0f), Vector3(-1.0f, 0.0f, 0.0f) },
        { Vector3(-1.0f, 1.0f, -1.0f),	Vector3(-1.0f, 0.0f, 0.0f) },
        { Vector3(-1.0f, 1.0f, 1.0f),	Vector3(-1.0f, 0.0f, 0.0f) },

        { Vector3(1.0f, -1.0f, 1.0f),	Vector3(1.0f, 0.0f, 0.0f) },// Normal X +
        { Vector3(1.0f, -1.0f, -1.0f),	Vector3(1.0f, 0.0f, 0.0f) },
        { Vector3(1.0f, 1.0f, -1.0f),	Vector3(1.0f, 0.0f, 0.0f) },
        { Vector3(1.0f, 1.0f, 1.0f),	Vector3(1.0f, 0.0f, 0.0f) },

        { Vector3(-1.0f, -1.0f, -1.0f), Vector3(0.0f, 0.0f, -1.0f) }, // Normal Z -
        { Vector3(1.0f, -1.0f, -1.0f),	Vector3(0.0f, 0.0f, -1.0f) },
        { Vector3(1.0f, 1.0f, -1.0f),	Vector3(0.0f, 0.0f, -1.0f) },
        { Vector3(-1.0f, 1.0f, -1.0f),	Vector3(0.0f, 0.0f, -1.0f) },

        { Vector3(-1.0f, -1.0f, 1.0f),	Vector3(0.0f, 0.0f, 1.0f) },// Normal Z +
        { Vector3(1.0f, -1.0f, 1.0f),	Vector3(0.0f, 0.0f, 1.0f) },
        { Vector3(1.0f, 1.0f, 1.0f),	Vector3(0.0f, 0.0f, 1.0f) },
        { Vector3(-1.0f, 1.0f, 1.0f),	Vector3(0.0f, 0.0f, 1.0f) },
    };

    WORD indices[] =
    {
        3,1,0, 2,1,3,
        6,4,5, 7,4,6,
        11,9,8, 10,9,11,
        14,12,13, 15,12,14,
        19,17,16, 18,17,19,
        22,20,21, 23,20,22
    };

    // Create Vertex Buffer
    D3D11_BUFFER_DESC vbDesc = {};
    vbDesc.ByteWidth = sizeof(Vertex) * ARRAYSIZE(vertices);
    vbDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vbDesc.Usage = D3D11_USAGE_DEFAULT;
    vbDesc.CPUAccessFlags = 0;

    D3D11_SUBRESOURCE_DATA vbData = {};
    vbData.pSysMem = vertices;
    HR_T(m_pDevice->CreateBuffer(&vbDesc, &vbData, &m_pVertexBuffer));

    m_VertexBufferStride = sizeof(Vertex);
    m_VertexBufferOffset = 0;

    // Create Index Buffer
    m_nIndices = ARRAYSIZE(indices);
    vbDesc.ByteWidth = sizeof(WORD) * m_nIndices;
    vbDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    vbDesc.Usage = D3D11_USAGE_DEFAULT;
    vbDesc.CPUAccessFlags = 0;

    D3D11_SUBRESOURCE_DATA ibData = {};
    ibData.pSysMem = indices;

    HR_T(m_pDevice->CreateBuffer(&vbDesc, &ibData, &m_pIndexBuffer));

    /*--------Vertex Shader--------*/
    ID3DBlob* vertexShaderBuffer = nullptr;
    //CompileShaderFromFile(L"BasicVertexShader.hlsl", "main", "vs_4_0", &vertexShaderBuffer);
    CompileShaderFromFile(L"BasicVertexShader.hlsl", "main", "vs_4_0", &vertexShaderBuffer);

    HR_T(m_pDevice->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(),
        vertexShaderBuffer->GetBufferSize(), NULL, &m_pVertexShader));

    /*--------InputLayout--------*/
    D3D11_INPUT_ELEMENT_DESC layout[] =
    {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0}
        //{"COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, offsetof(Vertex, color), D3D11_INPUT_PER_VERTEX_DATA, 0}
    };

    HR_T(m_pDevice->CreateInputLayout(layout, ARRAYSIZE(layout), vertexShaderBuffer->GetBufferPointer(),
        vertexShaderBuffer->GetBufferSize(), &m_pInputLayout));

    SAFE_RELEASE(vertexShaderBuffer);

    /*--------Pixel Shader Stage--------*/
    ID3DBlob* pixelShaderBuffer = nullptr;
    //CompileShaderFromFile(L"BasicPixelShader.hlsl", "main", "ps_4_0", &pixelShaderBuffer);
    CompileShaderFromFile(L"BasicPixelShader.hlsl", "main", "ps_4_0", &pixelShaderBuffer);

    HR_T(m_pDevice->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(),
        pixelShaderBuffer->GetBufferSize(), NULL, &m_pPixelShader));

    SAFE_RELEASE(pixelShaderBuffer);

    HR_T(CompileShaderFromFile(L"SolidPixelShader.hlsl", "main", "ps_4_0", &pixelShaderBuffer));
    HR_T(m_pDevice->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(),
        pixelShaderBuffer->GetBufferSize(), NULL, &m_pPixelShaderSolid));
    SAFE_RELEASE(pixelShaderBuffer);

    vbDesc.Usage = D3D11_USAGE_DEFAULT;
    vbDesc.ByteWidth = sizeof(ConstantBuffer);
    vbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    vbDesc.CPUAccessFlags = 0;
    HR_T(m_pDevice->CreateBuffer(&vbDesc, nullptr, &m_pConstantBuffer));

    //m_World = XMMatrixIdentity();

    //XMVECTOR Eye = XMVectorSet(0, 1, -15, 0);
    //XMVECTOR At = XMVectorSet(0, 1, 0, 0);
    //XMVECTOR Up = XMVectorSet(0, 1, 0, 0);

    //m_View = XMMatrixLookAtLH(Eye, At, Up);

    //// 투영 변환(절두체를 이해하면 된다.)
    //// 그려지는 범위 NearZ, FarZ값으로 설정
    //m_Projection = XMMatrixPerspectiveFovLH(XM_PIDIV2, m_ClientWidth / (FLOAT)m_ClientHeight, 0.01f, 100.0f);

    m_World = XMMatrixIdentity();
    XMVECTOR Eye = XMVectorSet(0.0f, 4.0f, -10.0f, 0.0f);
    XMVECTOR At = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
    XMVECTOR Up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
    m_View = XMMatrixLookAtLH(Eye, At, Up);
    m_Projection = XMMatrixPerspectiveFovLH(XM_PIDIV2 / 2.0f , m_ClientWidth / (FLOAT)m_ClientHeight, 0.01f, 100.0f);

    return true;
}

void TutorialApp::UninitScene()
{
    SAFE_RELEASE(m_pVertexBuffer);
    SAFE_RELEASE(m_pIndexBuffer);
    SAFE_RELEASE(m_pInputLayout);
    SAFE_RELEASE(m_pVertexShader);
    SAFE_RELEASE(m_pPixelShader);
    SAFE_RELEASE(m_pConstantBuffer);
}