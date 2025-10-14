#include "TutorialApp.h"
#include "Mesh.h"
#include "../Common/Helper.h"
#include <dxgi1_3.h>
#include <d3dcompiler.h>
#include <wrl/client.h>
#include <Directxtk/DDSTextureLoader.h>
#include <directxtk/WICTextureLoader.h>

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib") // 셰이더 컴파일 시 필요

struct Skybox
{
    Vector3 Pos;       // 스카이박스는 위치만 필요
};

struct SkyBoxCB
{
    Matrix mView;
    Matrix mProjection;
};

TutorialApp::TutorialApp(HINSTANCE hInstance) : GameApp(hInstance)
{

}

TutorialApp::~TutorialApp()
{
    UninitD3D();
    UninitImGUI();
    UninitScene();

    CheckDXGIDebug();
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
    __super::Update();

    // Light
    m_InitialLightDirs = { m_ImGuiManager.lightDir.x, m_ImGuiManager.lightDir.y, m_ImGuiManager.lightDir.z, 0.0f };
    m_LightDirsEvaluated = m_InitialLightDirs;
    m_AmbientColor = { m_ImGuiManager.ambientColor };
    m_DiffuseColor = { m_ImGuiManager.diffuseColor };
    m_SpecularColor = { m_ImGuiManager.specularColor };

    float shininess = { m_ImGuiManager.shininess };

    // Camera
    m_cameraPos = { m_Camera.GetPosition().x, m_Camera.GetPosition().y, m_Camera.GetPosition().z, 1 };
    float nearZ = m_ImGuiManager.depth.x;
    float farZ = m_ImGuiManager.depth.y;
    const float minZ = 0.1f;

    float finalFarZ = farZ;
    if (farZ - nearZ <= minZ)
    {
        finalFarZ = nearZ + minZ;
    }
    else
    {
        finalFarZ = farZ;
    }

    // 스케일 변환
    float scale= m_ImGuiManager.GetObjectScale();
    Vector3 scaleValue = { scale, scale, scale };

    // 회전각 변환
    Vector2 rotateValue = m_ImGuiManager.objectRotate / 180 * XM_PI;

    // 카메라 뷰에 적용
    m_Projection = XMMatrixPerspectiveFovLH(XM_PIDIV4 / m_ImGuiManager.FOV, m_ClientWidth / (FLOAT)m_ClientHeight, nearZ, finalFarZ);
    m_Camera.GetViewMatrix(m_View);

    if (m_ImGuiManager.viewChanger)
    {
        //m_pCubeTextureRV = m_pCubeDaylightTextureRV;
        m_pCubeTextureRV = m_pCubeHanakoTextureRV;
    }
    else
    {
        m_pCubeTextureRV = m_pCubeMuseumTextureRV;
    }

    Model* Zelda = GetModelByName("Zelda");
    if (Zelda)
    {
        Zelda->transform.position = { -100, 0, 0 };
        Zelda->transform.scale = { 1, 1, 1 };
        Zelda->transform.rotation = { rotateValue.y, rotateValue.x, 0 };

        Zelda->material.ambient = { 0, 0, 0, 0 };
        Zelda->material.diffuse = { 1, 1, 1, 1 };
        Zelda->material.specular = { 1, 1, 1, 1 };
        Zelda->material.shininess = { 1000 };
    }

    Model* Character = GetModelByName("Character");
    if (Character)
    {
        Character->transform.position = { 100, 0, 0 };
        Character->transform.scale = { 1, 1, 1 };

        Character->material.ambient = { 0, 0, 0, 0 };
        Character->material.diffuse = { 1, 1, 1, 1 };
        Character->material.specular = { 1, 1, 1, 1 };
        Character->material.shininess = { 1000 };
    }

    Model* Tree = GetModelByName("Tree");
    if (Tree)
    {
        Tree->transform.position = { 0, 0, 0 };
        Tree->transform.scale = { scaleValue };
        Tree->transform.rotation = {rotateValue.y, rotateValue.x, 0};

        Tree->material.ambient = { 0, 0, 0, 0 };
        Tree->material.diffuse = { 1, 1, 1, 1 };
        Tree->material.specular = { 1, 1, 1, 1 };
        Tree->material.shininess = { shininess };
    }
}

void TutorialApp::Render()
{
    // 1. Clear
    float color[4] = { 0.0f, 0.7f, 0.7f, 1.0f };

    float blendFactor[4] = { 0.f, 0.f, 0.f, 0.f }; // 일반적으로 0,0,0,0
    UINT sampleMask = 0xffffffff; // 모든 샘플 사용

    m_pDeviceContext->OMSetBlendState(m_pBlendState, blendFactor, sampleMask);

    m_pDeviceContext->OMSetRenderTargets(1, &m_pRenderTargetView, m_pDepthStencilView);
    m_pDeviceContext->ClearRenderTargetView(m_pRenderTargetView, color);
    m_pDeviceContext->ClearDepthStencilView(m_pDepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);

    // 2. 스카이박스 렌더
    m_pDeviceContext->OMSetDepthStencilState(m_pSkyboxDepthStencilState, 0);

    m_pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    m_pDeviceContext->IASetVertexBuffers(0, 1, &m_pSkyboxVertexBuffer, &m_SkyboxVertexBufferStride, &m_SkyboxVertexBufferOffset);
    m_pDeviceContext->IASetIndexBuffer(m_pSkyboxIndexBuffer, DXGI_FORMAT_R16_UINT, 0);
    m_pDeviceContext->IASetInputLayout(m_pSkyboxInputLayout);

    m_pDeviceContext->VSSetShader(m_pSkyboxVertexShader, nullptr, 0);
    m_pDeviceContext->PSSetShader(m_pSkyboxPixelShader, nullptr, 0);

    // 스카이박스 상수버퍼
    // 카메라 위치 중심
    SkyBoxCB cbSky;
    cbSky.mView = XMMatrixTranspose(XMMatrixScaling(10, 10, 10) * m_Camera.GetViewMatrixNoTranslation(m_View));
    cbSky.mProjection = XMMatrixTranspose(m_Projection);

    m_pDeviceContext->VSSetConstantBuffers(1, 1, &m_pSkyboxConstantBuffer);
    m_pDeviceContext->PSSetConstantBuffers(1, 1, &m_pConstantBuffer);
    m_pDeviceContext->UpdateSubresource(m_pSkyboxConstantBuffer, 0, nullptr, &cbSky, 0, 0);

    m_pDeviceContext->PSSetShaderResources(1, 1, &m_pCubeTextureRV);
    m_pDeviceContext->PSSetSamplers(1, 1, &m_pSamplerLinear);

    m_pDeviceContext->DrawIndexed(m_nSkyboxIndices, 0, 0);

    // 원래 상태 복원
    m_pDeviceContext->OMSetDepthStencilState(nullptr, 0);

    // 3. 일반 오브젝트 렌더
    m_pDeviceContext->IASetInputLayout(m_pInputLayout);
    m_pDeviceContext->VSSetShader(m_pVertexShader, nullptr, 0);
    m_pDeviceContext->PSSetShader(m_pPixelShader, nullptr, 0);

    // 특정 모델마다 다르게 설정한다면, 렌더에서 새로 모델 드로우 할때마다 설정을 바꿔주면 됨
    // 렌더링 전에 상태 적용
    m_pDeviceContext->RSSetState(m_pRasterStateNoCull);

    //RSSetState(None);

    // 투명 오브젝트와 불투명 오브젝트를 따로 렌더해야 한다?
    // 불투명 오브젝트 렌더 -> 알파 소팅 -> 투명 오브젝트 렌더
    for (auto& model : m_ModelLoader.models_)
    {
        ConstantBuffer cbObj;

        model.Draw(
            m_pDeviceContext,
            m_pConstantBuffer,
            m_View,
            m_Projection,
            m_LightDirsEvaluated,
            m_AmbientColor,
            m_DiffuseColor,
            m_SpecularColor,
            m_shininess,
            m_cameraPos,
            m_ImGuiManager.useLighting
        );
    }

    m_pDeviceContext->PSSetSamplers(0, 1, &m_pSamplerLinear);

    // 4. GUI 렌더
    m_ImGuiManager.Render();

    // 5. Present
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

    hr = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, D3D11_CREATE_DEVICE_DEBUG, NULL, NULL,
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
    depthDesc.Format = DXGI_FORMAT_D32_FLOAT;
    depthDesc.SampleDesc.Count = 1;
    depthDesc.Usage = D3D11_USAGE_DEFAULT;
    depthDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;

    ID3D11Texture2D* depthStencilBuffer = nullptr;
    m_pDevice->CreateTexture2D(&depthDesc, nullptr, &depthStencilBuffer);

    D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
    dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
    dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;

    m_pDevice->CreateDepthStencilView(depthStencilBuffer, &dsvDesc, &m_pDepthStencilView);

    // 깊이 테스트 - 오브젝트 렌더 시 DepthEnable = true 상태로 쓰고 있음
    D3D11_DEPTH_STENCIL_DESC dsDesc = {};
    dsDesc.DepthEnable = true;
    dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    dsDesc.DepthFunc = D3D11_COMPARISON_LESS;

    ID3D11DepthStencilState* depthStencilState;
    m_pDevice->CreateDepthStencilState(&dsDesc, &depthStencilState);
    m_pDeviceContext->OMSetDepthStencilState(depthStencilState, 0);

    D3D11_DEPTH_STENCIL_DESC skyDesc = {};
    skyDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    // Skybox는 Depth를 기록하지 않으므로 Enable false
    //skyDesc.DepthEnable = false;
    skyDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;

    m_pDevice->CreateDepthStencilState(&skyDesc, &m_pSkyboxDepthStencilState);

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

    D3D11_BLEND_DESC blendDesc = {};
    blendDesc.AlphaToCoverageEnable = FALSE; // 멀티샘플링과 함께 알파 투명도 사용 시 TRUE
    blendDesc.IndependentBlendEnable = FALSE; // 여러 렌더타깃에 각각 다른 블렌딩 적용 여부

    blendDesc.RenderTarget[0].BlendEnable = TRUE;                 // 블렌딩 활성화
    blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;   // 원본 색상 비율
    blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA; // 대상 색상 비율
    blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;       // 합산 방식
    blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;   // 알파 블렌딩 원본
    blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO; // 알파 블렌딩 대상
    blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
    blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

    HR_T(m_pDevice->CreateBlendState(&blendDesc, &m_pBlendState));

    D3D11_RASTERIZER_DESC rasterDesc = {};
    rasterDesc.FillMode = D3D11_FILL_SOLID;       // 실선으로 채움, D3D11_FILL_WIREFRAME 가능
    rasterDesc.FrontCounterClockwise = false;    // 정면이 시계방향인지 여부
    rasterDesc.DepthClipEnable = true;           // Z 클리핑 활성화

    // 뒷면 컬링(기본값)
    rasterDesc.CullMode = D3D11_CULL_BACK;
    m_pRasterStateBackCull = nullptr;
    m_pDevice->CreateRasterizerState(&rasterDesc, &m_pRasterStateBackCull);

    // 뒷면 컬링 없음
    rasterDesc.CullMode = D3D11_CULL_NONE;
    m_pRasterStateNoCull = nullptr;
    HR_T(m_pDevice->CreateRasterizerState(&rasterDesc, &m_pRasterStateNoCull));

    // 앞면 컬링
    rasterDesc.CullMode = D3D11_CULL_FRONT;
    m_pRasterStateFrontCull = nullptr;
    m_pDevice->CreateRasterizerState(&rasterDesc, &m_pRasterStateFrontCull);

    return true;
}

void TutorialApp::UninitD3D()
{
    SAFE_RELEASE(m_pRenderTargetView);
    SAFE_RELEASE(m_pDeviceContext);
    SAFE_RELEASE(m_pSwapChain);
    SAFE_RELEASE(m_pDevice);
    SAFE_RELEASE(m_pBlendState);
}

bool TutorialApp::InitImGUI()
{
    m_ImGuiManager.Initialize();
    m_ImGuiManager.BeginFrame(this->m_pDevice, this->m_pDeviceContext);
    return true;
}

void TutorialApp::UninitImGUI()
{
    m_ImGuiManager.Release();
}

bool TutorialApp::InitScene()
{
    HRESULT hr = 0;
    ID3D10Blob* errorMessage = nullptr;

    //m_ModelLoader.Load(m_pDevice, m_pDeviceContext, "../Resource/Character.fbx", "Character");
    m_ModelLoader.Load(m_pDevice, m_pDeviceContext, "../Resource/zeldaPosed001.fbx", "Zelda");
    m_ModelLoader.Load(m_pDevice, m_pDeviceContext, "../Resource/Tree.fbx", "Tree");

    Skybox skybox[] =
    {
        { Vector3(-1.0f,  1.0f,  1.0f) },
        { Vector3(1.0f,  1.0f,  1.0f) },
        { Vector3(1.0f, -1.0f,  1.0f) },
        { Vector3(-1.0f, -1.0f,  1.0f) },

        { Vector3(-1.0f,  1.0f, -1.0f) },
        { Vector3(1.0f,  1.0f, -1.0f) },
        { Vector3(1.0f, -1.0f, -1.0f) },
        { Vector3(-1.0f, -1.0f, -1.0f) },
    };

    WORD skyboxIndices[] =
    {
        0, 1, 2, 0, 2, 3,
        4, 6, 5, 4, 7, 6,
        4, 0, 3, 4, 3, 7,
        1, 5, 6, 1, 6, 2,
        4, 5, 1, 4, 1, 0,
        3, 2, 6, 3, 6, 7,
    };

    // Create Vertex Buffer
    D3D11_BUFFER_DESC vbDesc = {};

    /*--------Vertex Shader--------*/
    ID3DBlob* vertexShaderBuffer = nullptr;
    //CompileShaderFromFile(L"BasicVertexShader.hlsl", "main", "vs_4_0", &vertexShaderBuffer);
    CompileShaderFromFile(L"Shader/BasicVertexShader.hlsl", "main", "vs_4_0", &vertexShaderBuffer);

    HR_T(m_pDevice->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(),
        vertexShaderBuffer->GetBufferSize(), NULL, &m_pVertexShader));

    /*--------InputLayout--------*/
    D3D11_INPUT_ELEMENT_DESC layout[] =
    {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"BITANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 44, D3D11_INPUT_PER_VERTEX_DATA, 0},
    };

    HR_T(m_pDevice->CreateInputLayout(layout, ARRAYSIZE(layout), vertexShaderBuffer->GetBufferPointer(),
        vertexShaderBuffer->GetBufferSize(), &m_pInputLayout));

    SAFE_RELEASE(vertexShaderBuffer);

    /*--------Pixel Shader Stage--------*/
    ID3DBlob* pixelShaderBuffer = nullptr;
    CompileShaderFromFile(L"Shader/BasicPixelShader.hlsl", "main", "ps_4_0", &pixelShaderBuffer);

    HR_T(m_pDevice->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(),
        pixelShaderBuffer->GetBufferSize(), NULL, &m_pPixelShader));

    SAFE_RELEASE(pixelShaderBuffer);

    vbDesc.Usage = D3D11_USAGE_DEFAULT;
    vbDesc.ByteWidth = sizeof(ConstantBuffer);
    vbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    vbDesc.CPUAccessFlags = 0;
    HR_T(m_pDevice->CreateBuffer(&vbDesc, nullptr, &m_pConstantBuffer));

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
    HR_T(m_pDevice->CreateSamplerState(&sampDesc, &m_pSamplerLinear));

    // 여기에 텍스처 렌더, CreateDDSTextureFromFile 사용
    //HR_T(CreateDDSTextureFromFile(m_pDevice, L"../Resource/seafloor.dds", nullptr, &m_pTextureRV));

    //HR_T(CreateWICTextureFromFile(m_pDevice, L"../Resource/Bricks059_1K-JPG_Color.jpg", nullptr, &m_pTextureRV));

    // 노멀맵을 가져온다.
    //HR_T(CreateWICTextureFromFile(m_pDevice, L"../Resource/Bricks059_1K-JPG_NormalDX.jpg", nullptr, &m_pNormalMapRV));

    // 스펙큘러맵을 가져온다.
    //HR_T(CreateWICTextureFromFile(m_pDevice, L"../Resource/Bricks059_Specular.png", nullptr, &m_pSpecularMapRV));


    // 스카이박스 전용 버퍼 생성
    // Create Vertex Buffer
    D3D11_BUFFER_DESC skyVBDesc = {};
    skyVBDesc.ByteWidth = sizeof(Skybox) * ARRAYSIZE(skybox);
    skyVBDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    skyVBDesc.Usage = D3D11_USAGE_DEFAULT;
    skyVBDesc.CPUAccessFlags = 0;

    D3D11_SUBRESOURCE_DATA skyVBData = {};
    skyVBData.pSysMem = skybox;
    HR_T(m_pDevice->CreateBuffer(&skyVBDesc, &skyVBData, &m_pSkyboxVertexBuffer));

    m_SkyboxVertexBufferStride = sizeof(Skybox);
    m_SkyboxVertexBufferOffset = 0;

    // Create Index Buffer
    m_nSkyboxIndices = ARRAYSIZE(skyboxIndices);
    skyVBDesc.ByteWidth = sizeof(WORD) * m_nSkyboxIndices;
    skyVBDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    skyVBDesc.Usage = D3D11_USAGE_DEFAULT;
    skyVBDesc.CPUAccessFlags = 0;

    D3D11_SUBRESOURCE_DATA skyIBData = {};
    skyIBData.pSysMem = skyboxIndices;

    HR_T(m_pDevice->CreateBuffer(&skyVBDesc, &skyIBData, &m_pSkyboxIndexBuffer));

    ID3DBlob* vsBlobSkybox = nullptr;
    CompileShaderFromFile(L"Shader/SkyboxVertexShader.hlsl", "main", "vs_4_0", &vsBlobSkybox);

    HR_T(m_pDevice->CreateVertexShader(vsBlobSkybox->GetBufferPointer(),
        vsBlobSkybox->GetBufferSize(), NULL, &m_pSkyboxVertexShader));

    D3D11_INPUT_ELEMENT_DESC skylayout[] =
    {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
    };

    HR_T(m_pDevice->CreateInputLayout(skylayout, ARRAYSIZE(skylayout), vsBlobSkybox->GetBufferPointer(),
        vsBlobSkybox->GetBufferSize(), &m_pSkyboxInputLayout));

    SAFE_RELEASE(vsBlobSkybox);

    ID3DBlob* psBlobSkybox = nullptr;
    CompileShaderFromFile(L"Shader/SkyboxPixelShader.hlsl", "main", "ps_4_0", &psBlobSkybox);
    HR_T(m_pDevice->CreatePixelShader(psBlobSkybox->GetBufferPointer(),
        psBlobSkybox->GetBufferSize(), NULL, &m_pSkyboxPixelShader));

    SAFE_RELEASE(psBlobSkybox);

    skyVBDesc.Usage = D3D11_USAGE_DEFAULT;
    skyVBDesc.ByteWidth = sizeof(ConstantBuffer);
    skyVBDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    skyVBDesc.CPUAccessFlags = 0;
    HR_T(m_pDevice->CreateBuffer(&skyVBDesc, nullptr, &m_pSkyboxConstantBuffer));

    // Skybox 파일 로드
    HR_T(CreateDDSTextureFromFile(m_pDevice, L"../Resource/cubemap.dds", nullptr, &m_pCubeMuseumTextureRV));
    HR_T(CreateDDSTextureFromFile(m_pDevice, L"../Resource/Daylight.dds", nullptr, &m_pCubeDaylightTextureRV));
    HR_T(CreateDDSTextureFromFile(m_pDevice, L"../Resource/Hanako.dds", nullptr, &m_pCubeHanakoTextureRV));

    // 투영 변환(절두체를 이해하면 된다.)
    // 그려지는 범위 NearZ, FarZ값으로 설정
    m_Projection = XMMatrixPerspectiveFovLH(XM_PIDIV2, m_ClientWidth / (FLOAT)m_ClientHeight, 0.01f, 100.0f);

    m_World = XMMatrixIdentity();
    XMVECTOR Eye = XMVectorSet(0.0f, 4.0f, -10.0f, 0.0f);
    XMVECTOR At = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
    XMVECTOR Up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
    m_View = XMMatrixLookAtLH(Eye, At, Up);
    m_Projection = XMMatrixPerspectiveFovLH(XM_PIDIV2 / 2.0f, m_ClientWidth / (FLOAT)m_ClientHeight, 0.01f, 100.0f);

    m_LightDirsEvaluated = m_InitialLightDirs;

    return true;
}

void TutorialApp::UninitScene()
{
    SAFE_RELEASE(m_pInputLayout);
    SAFE_RELEASE(m_pVertexShader);
    SAFE_RELEASE(m_pPixelShader);
    SAFE_RELEASE(m_pSkyboxVertexShader);
    SAFE_RELEASE(m_pSkyboxPixelShader);
    SAFE_RELEASE(m_pConstantBuffer);
    SAFE_RELEASE(m_pRasterStateNoCull);
    SAFE_RELEASE(m_pRasterStateBackCull);
    SAFE_RELEASE(m_pRasterStateFrontCull);

    m_ModelLoader.Close();
}