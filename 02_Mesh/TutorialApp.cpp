#include "TutorialApp.h"
#include "../Common/Helper.h"
#include <dxgi1_3.h>
#include <d3dcompiler.h>
#include <wrl/client.h>

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib") // ���̴� ������ �� �ʿ�

using namespace Microsoft::WRL;
using namespace DirectX::SimpleMath;

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
    m_ParentObj.local = XMMatrixRotationY(XMConvertToRadians(m_ParentAngle))
        * XMMatrixTranslation(m_GUI.m_ParentWorldX, m_GUI.m_ParentWorldY, m_GUI.m_ParentWorldZ);
    m_ChildObj1.local = XMMatrixRotationY(XMConvertToRadians(m_Child1Angle))
        * XMMatrixTranslation(m_GUI.m_Child1LocalX, m_GUI.m_Child1LocalY, m_GUI.m_Child1LocalZ);
    m_ChildObj2.local = XMMatrixRotationY(XMConvertToRadians(m_Child2Angle))
        * XMMatrixTranslation(m_GUI.m_Child2LocalX, m_GUI.m_Child2LocalY, m_GUI.m_Child2LocalZ);

    //m_World = XMMatrixRotationY(XMConvertToRadians(m_Angle));
    
    m_ParentAngle += 0.001f;

    m_Child1Angle += 0.001f;
    m_Child2Angle += 0.001f;

    float PosX = m_GUI.playerPosX;
    float PosY = m_GUI.playerPosY;
    float PosZ = m_GUI.playerPosZ;

    float nearZ = m_GUI.nearZ;
    float farZ = m_GUI.farZ;

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

    // �����ϰ� ��ǥ ����
    if (m_GUI.isFocusParent)
    {
        float dz = m_GUI.m_ParentWorldZ - PosZ;

        if (fabs(dz) < minZ)
        {
            if (dz >= 0) dz = minZ;
            else dz = -minZ;
        }

        At = XMVectorSet(m_GUI.m_ParentWorldX, m_GUI.m_ParentWorldY, PosZ + dz, 0);
    }
    else
    {
        //float targetZ = 0.0f;

        //if (fabs(PosZ-targetZ) < minZ)
        //{
        //    if (PosZ >= 0) targetZ = PosZ - minZ;
        //    else targetZ = PosZ + minZ;
        //}

        At = XMVectorSet(PosX, PosY, PosZ + finalFarZ, 0);
    }

    XMVECTOR Up = XMVectorSet(0, 1, 0, 0);

    m_View = XMMatrixLookAtLH(Eye, At, Up);

    // XM_PI = 180��
    // ������ �þ߰��� 180����� ���� Eye�� �������� ���� 180��, �Ʒ��� 180��(-180��) ��� �ǹ�.
    //XMMatrixPerspectiveFovLH �Լ� ������ ��������� tan(FovY/2)=tan(��/2)=�� �̹Ƿ� ȭ�鿡 ǥ�õ��� ����.
    m_Projection = XMMatrixPerspectiveFovLH(XM_PIDIV2 / m_GUI.FOV, m_ClientWidth / (FLOAT)m_ClientHeight, nearZ, finalFarZ);

    UpdateWorld(m_ParentObj);
}

void TutorialApp::Render()
{
    float color[4] = { 0.5f, 0.5f, 0.5f, 1.0f };

    m_pDeviceContext->OMSetRenderTargets(1, &m_pRenderTargetView, m_pDepthStencilView);
    m_pDeviceContext->ClearRenderTargetView(m_pRenderTargetView, color);
    m_pDeviceContext->ClearDepthStencilView(m_pDepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);

    /*--------1. Input-Assembler--------*/
    m_pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    m_pDeviceContext->IASetInputLayout(m_pInputLayout);

    //m_pDeviceContext->IASetVertexBuffers(0, 1, &m_ParentObj.m_pVertexBuffer, &m_VertexBufferStride, &m_VertexBufferOffset);
    //m_pDeviceContext->IASetIndexBuffer(m_ParentObj.m_pIndexBuffer, DXGI_FORMAT_R16_UINT, m_IndexBufferOffset);

    // ���̴� ����
    m_pDeviceContext->VSSetShader(m_pVertexShader, nullptr, 0);
    m_pDeviceContext->PSSetShader(m_pPixelShader, nullptr, 0);
    m_pDeviceContext->VSSetConstantBuffers(0, 1, &m_pConstantBuffer);

    // ���� ������Ʈ���� ����ǵ��� �Ͽ���.
    //m_pDeviceContext->Draw(m_VertexCount, 0);
    //m_pDeviceContext->DrawIndexed(m_IndexCount, 0, 0);

    RenderObject(m_pDeviceContext, m_ParentObj);

    // GUI�� �� ���߿� �����ϵ��� �Ѵ�.
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
        MessageBox(m_hWnd, L"����ü�� ���� ����", L"����", MB_OK);
        return false;
    }

    ID3D11Texture2D* pBackBufferTexture = nullptr;
    hr = m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&pBackBufferTexture);
    if (FAILED(hr))
    {
        MessageBox(m_hWnd, L"����� ȹ�� ����", L"����", MB_OK);
        return false;
    }

    //-------Create D3D11 Device, DeviceContext-------//
    hr = m_pDevice->CreateRenderTargetView(pBackBufferTexture, NULL, &m_pRenderTargetView);
    if (FAILED(hr))
    {
        MessageBox(m_hWnd, L"���� Ÿ�� �� ���� ����", L"����", MB_OK);
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

    // ���� �׽�Ʈ
    D3D11_DEPTH_STENCIL_DESC dsDesc = {};
    dsDesc.DepthEnable = true;
    dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    dsDesc.DepthFunc = D3D11_COMPARISON_LESS;

    ID3D11DepthStencilState* depthStencilState;
    m_pDevice->CreateDepthStencilState(&dsDesc, &depthStencilState);
    m_pDeviceContext->OMSetDepthStencilState(depthStencilState, 0);

    m_pDeviceContext->OMSetRenderTargets(1, &m_pRenderTargetView, NULL);

    /*-------Create RenderTarget View-------*/
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

    Vertex pVertices[] =
    {
        { Vector3(-1.0f, 1.0f, -1.0f),	Vector4(0.0f, 0.0f, 1.0f, 1.0f) },
        { Vector3(1.0f, 1.0f, -1.0f),	Vector4(0.0f, 1.0f, 0.0f, 1.0f) },
        { Vector3(1.0f, 1.0f, 1.0f),	Vector4(0.0f, 1.0f, 1.0f, 1.0f) },
        { Vector3(-1.0f, 1.0f, 1.0f),	Vector4(1.0f, 0.0f, 0.0f, 1.0f) },
        { Vector3(-1.0f, -1.0f, -1.0f), Vector4(1.0f, 0.0f, 1.0f, 1.0f) },
        { Vector3(1.0f, -1.0f, -1.0f),	Vector4(1.0f, 1.0f, 0.0f, 1.0f) },
        { Vector3(1.0f, -1.0f, 1.0f),	Vector4(1.0f, 1.0f, 1.0f, 1.0f) },
        { Vector3(-1.0f, -1.0f, 1.0f),	Vector4(0.0f, 0.0f, 0.0f, 1.0f) },
    };

    WORD pIndices[] =
    {
        3,1,0, 2,1,3,
        0,5,4, 1,5,0,
        3,4,7, 0,4,3,
        1,6,5, 2,6,1,
        2,7,6, 3,7,2,
        6,4,5, 7,4,6,
    };

    Vertex cVertices[] =
    {
        // �Ƕ�̵� ������ (�ظ� �簢�� + �����)
        { Vector3(-1.0f, 0.0f, -1.0f), Vector4(1.0f, 0.0f, 0.0f, 1.0f) }, // �ظ� ���� ��
        { Vector3(1.0f, 0.0f, -1.0f),  Vector4(0.0f, 1.0f, 0.0f, 1.0f) }, // �ظ� ������ ��
        { Vector3(1.0f, 0.0f, 1.0f),   Vector4(0.0f, 0.0f, 1.0f, 1.0f) }, // �ظ� ������ ��
        { Vector3(-1.0f, 0.0f, 1.0f),  Vector4(1.0f, 1.0f, 0.0f, 1.0f) }, // �ظ� ���� ��
        { Vector3(0.0f, 2.0f, 0.0f),   Vector4(1.0f, 0.0f, 1.0f, 1.0f) }  // �����
    };

    WORD cIndices[] =
    {
        // �ظ�
        0, 2, 1,  0, 3, 2,

        // ���� �ﰢ��
        0, 4, 1,  // �ո�
        1, 4, 2,  // ������ ��
        2, 4, 3,  // ���� ��
        3, 4, 0   // ���� ��
    };

    m_ParentObj.m_Vertices.insert(
        m_ParentObj.m_Vertices.end(),
        begin(pVertices), end(pVertices)
    );

    m_ParentObj.m_Indices.insert(
        m_ParentObj.m_Indices.end(),
        begin(pIndices), end(pIndices)
    );

    m_ChildObj1.m_Vertices.insert(
        m_ChildObj1.m_Vertices.end(),
        begin(cVertices), end(cVertices)
    );

    m_ChildObj1.m_Indices.insert(
        m_ChildObj1.m_Indices.end(),
        begin(cIndices), end(cIndices)
    );

    m_ChildObj2.m_Vertices.insert(
        m_ChildObj2.m_Vertices.end(),
        begin(cVertices), end(cVertices)
    );

    m_ChildObj2.m_Indices.insert(
        m_ChildObj2.m_Indices.end(),
        begin(cIndices), end(cIndices)
    );

    // Create Vertex Buffer
    D3D11_BUFFER_DESC vbDesc = {};
    //m_VertexCount = ARRAYSIZE(vertices);
    //vbDesc.ByteWidth = sizeof(Vertex) * m_VertexCount;
    vbDesc.ByteWidth = sizeof(Vertex) * m_ParentObj.m_Vertices.size();
    vbDesc.CPUAccessFlags = 0;
    vbDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vbDesc.MiscFlags = 0;
    vbDesc.Usage = D3D11_USAGE_DEFAULT;

    D3D11_SUBRESOURCE_DATA vbData = {};
    vbData.pSysMem = pVertices;
    HR_T(m_pDevice->CreateBuffer(&vbDesc, &vbData, &m_ParentObj.m_pVertexBuffer));

    m_VertexBufferStride = sizeof(Vertex);
    m_VertexBufferOffset = 0;

    // Create Index Buffer
    //m_IndexCount = ARRAYSIZE(indices);
    //vbDesc.ByteWidth = sizeof(WORD) * m_IndexCount;
    vbDesc.ByteWidth = sizeof(WORD) * m_ParentObj.m_Indices.size();
    vbDesc.Usage = D3D11_USAGE_DEFAULT;
    vbDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;

    D3D11_SUBRESOURCE_DATA ibData = {};
    ibData.pSysMem = pIndices;

    HR_T(m_pDevice->CreateBuffer(&vbDesc, &ibData, &m_ParentObj.m_pIndexBuffer));

    m_IndexBufferOffset = 0;

    // Create Vertex Buffer(Child)
    D3D11_BUFFER_DESC cvbDesc = {};
    //m_VertexCount = ARRAYSIZE(vertices);
    //vbDesc.ByteWidth = sizeof(Vertex) * m_VertexCount;
    cvbDesc.ByteWidth = sizeof(Vertex) * m_ChildObj1.m_Vertices.size();
    cvbDesc.CPUAccessFlags = 0;
    cvbDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    cvbDesc.MiscFlags = 0;
    cvbDesc.Usage = D3D11_USAGE_DEFAULT;

    D3D11_SUBRESOURCE_DATA cvbData = {};
    cvbData.pSysMem = cVertices;
    HR_T(m_pDevice->CreateBuffer(&cvbDesc, &cvbData, &m_ChildObj1.m_pVertexBuffer));
    m_ChildObj2.m_pVertexBuffer = m_ChildObj1.m_pVertexBuffer;

    m_VertexBufferStride = sizeof(Vertex);
    m_VertexBufferOffset = 0;

    // Create Index Buffer
    cvbDesc.ByteWidth = sizeof(WORD) * m_ChildObj1.m_Indices.size();
    cvbDesc.Usage = D3D11_USAGE_DEFAULT;
    cvbDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;

    D3D11_SUBRESOURCE_DATA cibData = {};
    cibData.pSysMem = cIndices;

    HR_T(m_pDevice->CreateBuffer(&cvbDesc, &cibData, &m_ChildObj1.m_pIndexBuffer));
    m_ChildObj2.m_pIndexBuffer = m_ChildObj1.m_pIndexBuffer;

    m_IndexBufferOffset = 0;

    /*--------Vertex Shader--------*/
    ID3DBlob* vertexShaderBuffer = nullptr;
    CompileShaderFromFile(L"..\\..\\D3D Class\\02_Mesh\\BasicVertexShader.hlsl", "main", "vs_4_0", &vertexShaderBuffer);

    HR_T(m_pDevice->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(),
        vertexShaderBuffer->GetBufferSize(), NULL, &m_pVertexShader));

    /*--------InputLayout--------*/
    D3D11_INPUT_ELEMENT_DESC layout[] =
    {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(Vertex, position), D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, offsetof(Vertex, color), D3D11_INPUT_PER_VERTEX_DATA, 0}
    };

    HR_T(m_pDevice->CreateInputLayout(layout, ARRAYSIZE(layout), vertexShaderBuffer->GetBufferPointer(),
        vertexShaderBuffer->GetBufferSize(), &m_pInputLayout));

    SAFE_RELEASE(vertexShaderBuffer);

    /*--------Pixel Shader Stage--------*/
    ID3DBlob* pixelShaderBuffer = nullptr;
    CompileShaderFromFile(L"..\\..\\D3D Class\\02_Mesh\\BasicPixelShader.hlsl", "main", "ps_4_0", &pixelShaderBuffer);

    HR_T(m_pDevice->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(),
        pixelShaderBuffer->GetBufferSize(), NULL, &m_pPixelShader));

    SAFE_RELEASE(pixelShaderBuffer);

    vbDesc.Usage = D3D11_USAGE_DEFAULT;
    vbDesc.ByteWidth = sizeof(ConstantBuffer);
    vbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    vbDesc.CPUAccessFlags = 0;
    HR_T(m_pDevice->CreateBuffer(&vbDesc, nullptr, &m_pConstantBuffer));
    m_World = XMMatrixIdentity();

    XMVECTOR Eye = XMVectorSet(0, 1, -15, 0);
    XMVECTOR At = XMVectorSet(0, 1, 0, 0);
    XMVECTOR Up = XMVectorSet(0, 1, 0, 0);

    m_View = XMMatrixLookAtLH(Eye, At, Up);

    // ���� ��ȯ(����ü�� �����ϸ� �ȴ�.)
    // �׷����� ���� NearZ, FarZ������ ����
    m_Projection = XMMatrixPerspectiveFovLH(XM_PIDIV2, m_ClientWidth / (FLOAT)m_ClientHeight, 0.01f, 100.0f);

    m_ParentObj.local = XMMatrixIdentity();
    m_ParentObj.local = XMMatrixRotationY(XMConvertToRadians(m_ParentAngle));

    m_ParentObj.AddChild(&m_ChildObj1);
    m_ChildObj1.AddChild(&m_ChildObj2);

    return true;
}

void TutorialApp::UninitScene()
{
    SAFE_RELEASE(m_ParentObj.m_pVertexBuffer);
    SAFE_RELEASE(m_ParentObj.m_pIndexBuffer);
    SAFE_RELEASE(m_pInputLayout);
    SAFE_RELEASE(m_pVertexShader);
    SAFE_RELEASE(m_pPixelShader);
    SAFE_RELEASE(m_pConstantBuffer);
}