#include "TutorialApp.h"
#include "Mesh.h"
#include "../Common/Helper.h"
#include "../Common/TimeSystem.h"
#include <dxgi1_3.h>
#include <d3dcompiler.h>
#include <wrl/client.h>
#include <Directxtk/DDSTextureLoader.h>
#include <directxtk/WICTextureLoader.h>

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib") // 셰이더 컴파일 시 필요

struct ShadowCB
{
    Matrix mView;
    Matrix mProjection;
};

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

    m_Time.Initialize();

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

    m_ImGuiManager.Update();

    // Light
    m_InitialLightDirs = { m_ImGuiManager.lightDir.x, m_ImGuiManager.lightDir.y, m_ImGuiManager.lightDir.z, 0.0f };
    m_LightDirsEvaluated = m_InitialLightDirs;

    m_Light.Direction = { m_ImGuiManager.lightDir.x, m_ImGuiManager.lightDir.y, m_ImGuiManager.lightDir.z  };

    m_AmbientColor = { m_ImGuiManager.ambientLight };
    m_DiffuseColor = { m_ImGuiManager.diffuseLight };
    m_SpecularColor = { m_ImGuiManager.specularLight };

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

    




    //SkeletalModel* BoxHuman = GetSkeletalModelByName("SkinningTest");
    //if (BoxHuman)
    //{
    //    Object obj = m_ImGuiManager.object1;

    //    BoxHuman->transform.position = { obj.transform.GetPosition() };
    //    BoxHuman->transform.scale = { obj.transform.GetScale() };
    //    BoxHuman->transform.rotation = { obj.transform.GetRotation() };

    //    BoxHuman->material.ambient = { obj.ambient };
    //    BoxHuman->material.diffuse = { obj.diffuse };
    //    BoxHuman->material.specular = { obj.specular };
    //    BoxHuman->material.shininess = { obj.shininess };
    //}

    //SkeletalModel* model = GetSkeletalModelByName("model");
    //if (model)
    //{
    //    Object obj = m_ImGuiManager.object2;

    //    model->transform.position = { obj.transform.GetPosition() };
    //    model->transform.scale = { obj.transform.GetScale() };
    //    model->transform.rotation = { obj.transform.GetRotation() };

    //    model->material.ambient = { obj.ambient };
    //    model->material.diffuse = { obj.diffuse };
    //    model->material.specular = { obj.specular };
    //    model->material.shininess = { obj.shininess };
    //}

    //m_SkeletalModelLoader.Update(m_Time.GetDeltaTime());
}

void TutorialApp::Render()
{
    Vector3 ObjectPos = { m_ImGuiManager.object1.transform.GetPosition().x, m_ImGuiManager.object1.transform.GetPosition().y ,m_ImGuiManager.object1.transform.GetPosition().z };

    // 1. Clear
    float color[4] = { 0.0f, 0.7f, 0.7f, 1.0f };
    float blendFactor[4] = { 0.f, 0.f, 0.f, 0.f }; // 일반적으로 0,0,0,0
    UINT sampleMask = 0xffffffff; // 모든 샘플 사용

    m_pDeviceContext->OMSetBlendState(m_pBlendState.Get(), blendFactor, sampleMask);
    ID3D11RenderTargetView* rtv = m_pRenderTargetView.Get(); // 내부 포인터
    m_pDeviceContext->OMSetRenderTargets(1, &rtv, m_pDepthStencilView.Get());
    m_pDeviceContext->ClearRenderTargetView(rtv, color);
    m_pDeviceContext->ClearDepthStencilView(m_pDepthStencilView.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);
    m_pDeviceContext->RSSetViewports(1, &m_MainViewport);



    // ShadowPass
    m_pDeviceContext->RSSetViewports(1, &m_ShadowViewport);
    m_pDeviceContext->OMSetRenderTargets(0, nullptr, m_pShadowMapDSV.Get());
    m_pDeviceContext->ClearDepthStencilView(m_pShadowMapDSV.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);
    m_pDeviceContext->VSSetShader(m_pShadowVS.Get(), nullptr, 0);
    m_pDeviceContext->PSSetShader(nullptr, nullptr, 0);

    // 상수 버퍼 업데이트
    ShadowCB cbShadow;
    cbShadow.mView = XMMatrixTranspose(m_ShadowView);
    cbShadow.mProjection = XMMatrixTranspose(m_ShadowProjection);
    m_pDeviceContext->VSSetConstantBuffers(0, 1, &m_pShadowConstantBuffer);
    m_pDeviceContext->UpdateSubresource(m_pShadowConstantBuffer, 0, nullptr, &cbShadow, 0, 0);

    m_pDeviceContext->RSSetViewports(1, &m_MainViewport);
    m_pDeviceContext->OMSetRenderTargets(1, &rtv, m_pDepthStencilView.Get());
    m_pDeviceContext->OMSetDepthStencilState(nullptr, 0);

    // 3. 일반 오브젝트 렌더
    m_pDeviceContext->IASetInputLayout(m_pInputLayout.Get());
    m_pDeviceContext->VSSetShader(m_pVertexShader.Get(), nullptr, 0);
    m_pDeviceContext->PSSetShader(m_pPixelShader.Get(), nullptr, 0);
    m_pDeviceContext->RSSetState(m_pRasterStateNoCull.Get());

    // Sampler
    m_pDeviceContext->PSSetSamplers(0, 1, &m_pSamplerLinear);
    m_pDeviceContext->PSSetSamplers(1, 1, &m_pSamplerLinear);

    // ShadowMapSRV
    m_pDeviceContext->PSSetShaderResources(5, 1, m_pShadowMapSRV.GetAddressOf());

    for (auto& section : m_StaticMesh.m_StaticMeshSection)
    {
        int matIdx = section.materialIndex;
        if (matIdx < 0 || matIdx >= m_StaticMesh.m_Materials.size())
            continue;

        Material& mat = m_StaticMesh.m_Materials[matIdx];

        ConstantBuffer cb;
        cb.mWorld = XMMatrixTranspose(XMMatrixScaling(10, 10, 10)) *
            XMMatrixTranspose(XMMatrixTranslation(ObjectPos.x, ObjectPos.y, ObjectPos.z))
            * XMMatrixTranspose(m_StaticMesh.m_World);
        cb.mView = XMMatrixTranspose(m_View);
        cb.mProjection = XMMatrixTranspose(m_Projection);
        cb.vLightDir = m_LightDirsEvaluated;

        // 머티리얼 정보
        cb.vMaterialAmbient = m_ImGuiManager.object1.ambient;
        cb.vMaterialDiffuse = m_ImGuiManager.object1.diffuse;
        cb.vMaterialSpecular = m_ImGuiManager.object1.specular;
        cb.vShininess = { m_ImGuiManager.object1.shininess };

        cb.vAmbientColor = m_AmbientColor;
        cb.vDiffuseColor = m_DiffuseColor;
        cb.vSpecularColor = m_SpecularColor;
        cb.cameraPos = m_cameraPos;
        cb.UseLighting = m_ImGuiManager.useLighting;

        // 텍스처 관련
        cb.hasTexture = mat.hasTexture ? 1 : 0;
        cb.solidColor = mat.hasTexture ? XMFLOAT4(1, 1, 1, 1) : mat.solidColor;
        cb.hasNormalMap = mat.hasNormalMap ? 1 : 0;
        cb.hasSpecularMap = mat.hasSpecularMap ? 1 : 0;
        cb.hasEmissiveMap = mat.hasEmissiveMap ? 1 : 0;

        cb.ShadowView = cbShadow.mView;
        cb.ShadowProjection = cbShadow.mProjection;

        cb.ShadowView = XMMatrixTranspose(m_ShadowView);
        cb.ShadowProjection = XMMatrixTranspose(m_ShadowProjection);

        // 상수버퍼 업데이트
        m_pDeviceContext->VSSetConstantBuffers(0, 1, &m_pStaticMeshConstantBuffer);
        m_pDeviceContext->PSSetConstantBuffers(0, 1, &m_pStaticMeshConstantBuffer);
        m_pDeviceContext->UpdateSubresource(m_pStaticMeshConstantBuffer, 0, nullptr, &cb, 0, 0);

        // Vertex/Index Buffer
        UINT stride = sizeof(Vertex);
        UINT offset = 0;
        m_pDeviceContext->IASetVertexBuffers(0, 1, &section.VertexBuffer, &stride, &offset);
        m_pDeviceContext->IASetIndexBuffer(section.IndexBuffer, DXGI_FORMAT_R16_UINT, 0);
        m_pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

        // PS 텍스처 바인딩
        ID3D11ShaderResourceView* nullSRV[1] = { nullptr };
        for (int i = 0; i <= 4; i++)
            m_pDeviceContext->PSSetShaderResources(i, 1, nullSRV);

        if (mat.hasTexture && mat.diffuseSRV)
            m_pDeviceContext->PSSetShaderResources(0, 1, &mat.diffuseSRV);
        if (mat.hasNormalMap && mat.normalSRV)
            m_pDeviceContext->PSSetShaderResources(2, 1, &mat.normalSRV);
        if (mat.hasSpecularMap && mat.specularSRV)
            m_pDeviceContext->PSSetShaderResources(3, 1, &mat.specularSRV);
        if (mat.hasEmissiveMap && mat.emissiveSRV)
            m_pDeviceContext->PSSetShaderResources(4, 1, &mat.emissiveSRV);

        // Draw 호출
        m_pDeviceContext->DrawIndexed(static_cast<UINT>(section.Indices.size()), 0, 0);
    }
    for (auto& section : ground.m_StaticMeshSection)
    {
        int matIdx = section.materialIndex;
        if (matIdx < 0 || matIdx >= ground.m_Materials.size())
            continue;

        Material& mat = ground.m_Materials[matIdx];

        ConstantBuffer cb;
        cb.mWorld = XMMatrixTranspose(XMMatrixTranslation(0, -100, 0)) * XMMatrixTranspose(ground.m_World);
        cb.mView = XMMatrixTranspose(m_View);
        cb.mProjection = XMMatrixTranspose(m_Projection);
        cb.vLightDir = m_LightDirsEvaluated;

        // 머티리얼 정보
        cb.vMaterialAmbient = mat.ambient;
        cb.vMaterialDiffuse = mat.diffuse;
        cb.vMaterialSpecular = mat.specular;
        cb.vShininess = mat.shininess;

        cb.vAmbientColor = m_AmbientColor;
        cb.vDiffuseColor = m_DiffuseColor;
        cb.vSpecularColor = m_SpecularColor;
        cb.cameraPos = m_cameraPos;
        cb.UseLighting = m_ImGuiManager.useLighting;

        // 텍스처 관련
        cb.hasTexture = mat.hasTexture ? 1 : 0;
        cb.solidColor = mat.hasTexture ? XMFLOAT4(1, 1, 1, 1) : mat.solidColor;
        cb.hasNormalMap = mat.hasNormalMap ? 1 : 0;
        cb.hasSpecularMap = mat.hasSpecularMap ? 1 : 0;
        cb.hasEmissiveMap = mat.hasEmissiveMap ? 1 : 0;

        cb.ShadowView = XMMatrixTranspose(m_ShadowView);
        cb.ShadowProjection = XMMatrixTranspose(m_ShadowProjection);

        // 상수버퍼 업데이트
        m_pDeviceContext->VSSetConstantBuffers(0, 1, &m_pStaticMeshConstantBuffer);
        m_pDeviceContext->PSSetConstantBuffers(0, 1, &m_pStaticMeshConstantBuffer);
        m_pDeviceContext->UpdateSubresource(m_pStaticMeshConstantBuffer, 0, nullptr, &cb, 0, 0);

        // Vertex/Index Buffer
        UINT stride = sizeof(Vertex);
        UINT offset = 0;
        m_pDeviceContext->IASetVertexBuffers(0, 1, &section.VertexBuffer, &stride, &offset);
        m_pDeviceContext->IASetIndexBuffer(section.IndexBuffer, DXGI_FORMAT_R16_UINT, 0);
        m_pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

        // PS 텍스처 바인딩
        ID3D11ShaderResourceView* nullSRV[1] = { nullptr };
        for (int i = 0; i <= 4; i++)
            m_pDeviceContext->PSSetShaderResources(i, 1, nullSRV);

        if (mat.hasTexture && mat.diffuseSRV)
            m_pDeviceContext->PSSetShaderResources(0, 1, &mat.diffuseSRV);
        if (mat.hasNormalMap && mat.normalSRV)
            m_pDeviceContext->PSSetShaderResources(2, 1, &mat.normalSRV);
        if (mat.hasSpecularMap && mat.specularSRV)
            m_pDeviceContext->PSSetShaderResources(3, 1, &mat.specularSRV);
        if (mat.hasEmissiveMap && mat.emissiveSRV)
            m_pDeviceContext->PSSetShaderResources(4, 1, &mat.emissiveSRV);

        // Draw 호출
        m_pDeviceContext->DrawIndexed(static_cast<UINT>(section.Indices.size()), 0, 0);
    }

    // 2. 스카이박스 렌더
    //m_pDeviceContext->OMSetDepthStencilState(m_pSkyboxDepthStencilState, 0);

    //m_pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    //m_pDeviceContext->IASetVertexBuffers(0, 1, &m_pSkyboxVertexBuffer, &m_SkyboxVertexBufferStride, &m_SkyboxVertexBufferOffset);
    //m_pDeviceContext->IASetIndexBuffer(m_pSkyboxIndexBuffer, DXGI_FORMAT_R16_UINT, 0);
    //m_pDeviceContext->IASetInputLayout(m_pSkyboxInputLayout.Get());
    //m_pDeviceContext->VSSetShader(m_pSkyboxVertexShader.Get(), nullptr, 0);
    //m_pDeviceContext->PSSetShader(m_pSkyboxPixelShader.Get(), nullptr, 0);

    //// 상수 버퍼 업데이트
    //SkyBoxCB cbSky;
    //cbSky.mView = XMMatrixTranspose(XMMatrixScaling(10, 10, 10) * m_Camera.GetViewMatrixNoTranslation(m_View));
    //cbSky.mProjection = XMMatrixTranspose(m_Projection);
    //m_pDeviceContext->VSSetConstantBuffers(1, 1, &m_pSkyboxConstantBuffer);
    //m_pDeviceContext->UpdateSubresource(m_pSkyboxConstantBuffer, 0, nullptr, &cbSky, 0, 0);
    //m_pDeviceContext->PSSetShaderResources(1, 1, &m_pCubeTextureRV);
    //m_pDeviceContext->PSSetSamplers(1, 1, &m_pSamplerLinear);
    //m_pDeviceContext->DrawIndexed(m_nSkyboxIndices, 0, 0);

    //// 원래 상태 복원
    //m_pDeviceContext->OMSetDepthStencilState(nullptr, 0);

    // 4. GUI 렌더
    m_ImGuiManager.BeginFrame();
    //m_ImGuiManager.RenderObjectUI("오브젝트1",object1)
    m_ImGuiManager.DrawObjectUI();
    m_ImGuiManager.DrawShadowSRV(m_pShadowMapSRV.Get(), (float)SHADOW_WIDTH, (float)SHADOW_HEIGHT);
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
    hr = m_pDevice->CreateRenderTargetView(pBackBufferTexture, NULL, m_pRenderTargetView.GetAddressOf());
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
    skyDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
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
    ID3D11RenderTargetView* rtv = m_pRenderTargetView.Get();  // 내부 포인터
    m_pDeviceContext->OMSetRenderTargets(1, &rtv, m_pDepthStencilView.Get());

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
    SAFE_RELEASE(m_pDeviceContext);
    SAFE_RELEASE(m_pSwapChain);
    SAFE_RELEASE(m_pDevice);
}

bool TutorialApp::InitImGUI()
{
    m_ImGuiManager.Initialize(this->m_pDevice, this->m_pDeviceContext);
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

    //m_SkeletalModelLoader.Load(m_pDevice, m_pDeviceContext, "../Resource/Vampire_SkinningTest.fbx", "model");
    //m_SkeletalModelLoader.Load(m_pDevice, m_pDeviceContext, "../Resource/SkinningTest.fbx", "SkinningTest");

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
    CompileShaderFromFile(L"Shader/BasicPixelShader.hlsl", "main", "ps_5_0", &pixelShaderBuffer);

    HR_T(m_pDevice->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(),
        pixelShaderBuffer->GetBufferSize(), NULL, &m_pPixelShader));

    SAFE_RELEASE(pixelShaderBuffer);

    vbDesc.Usage = D3D11_USAGE_DEFAULT;
    //vbDesc.ByteWidth = sizeof(ConstantBuffer);
    vbDesc.ByteWidth = (sizeof(ConstantBuffer) + 15) / 16 * 16; // 16바이트 정렬
    vbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    vbDesc.CPUAccessFlags = 0;
    HR_T(m_pDevice->CreateBuffer(&vbDesc, nullptr, &m_pStaticMeshConstantBuffer));

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



    m_StaticMesh.ReadFile(m_pDevice, "../Resource/Appearance Miku/Appearance Miku.fbx");
    //m_StaticMesh.ReadFile(m_pDevice, "../Resource/box.fbx");
    ground.ReadFile(m_pDevice, "../Resource/Ground.fbx");

    for (auto& section : m_StaticMesh.m_StaticMeshSection)
    {
        section.Create(m_pDevice);
    }

    for (auto& section : ground.m_StaticMeshSection)
    {
        section.Create(m_pDevice);
    }

    /*--------Vertex Shader--------*/
    ID3DBlob* boneShader = nullptr;
    CompileShaderFromFile(L"Shader/BoneModelVS.hlsl", "main", "vs_5_0", &boneShader);

    HR_T(m_pDevice->CreateVertexShader(boneShader->GetBufferPointer(),
        boneShader->GetBufferSize(), NULL, &m_pSkeletalVS));

    /*--------InputLayout--------*/
    D3D11_INPUT_ELEMENT_DESC bonelayout[] =
    {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"BITANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 44, D3D11_INPUT_PER_VERTEX_DATA, 0},

        {"BLENDINDICES", 0, DXGI_FORMAT_R32G32B32A32_SINT,   0, 56, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"BLENDWEIGHT",  0, DXGI_FORMAT_R32G32B32A32_FLOAT,  0, 72, D3D11_INPUT_PER_VERTEX_DATA, 0},
    };

    HR_T(m_pDevice->CreateInputLayout(bonelayout, ARRAYSIZE(bonelayout), boneShader->GetBufferPointer(),
        boneShader->GetBufferSize(), &m_pSkeletalInputLayout));

    SAFE_RELEASE(boneShader);

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

    m_MainViewport.TopLeftX = 0.0f;
    m_MainViewport.TopLeftY = 0.0f;
    m_MainViewport.Width = static_cast<float>(m_ClientWidth);
    m_MainViewport.Height = static_cast<float>(m_ClientHeight);
    m_MainViewport.MinDepth = 0.0f;
    m_MainViewport.MaxDepth = 1.0f;

    // ShadowMap
    m_ShadowViewport.TopLeftX = 0;
    m_ShadowViewport.TopLeftY = 0;
    m_ShadowViewport.Width = static_cast<float>(SHADOW_WIDTH);
    m_ShadowViewport.Height = static_cast<float>(SHADOW_HEIGHT);
    m_ShadowViewport.MinDepth = 0.0f;
    m_ShadowViewport.MaxDepth = 1.0f;

    D3D11_BUFFER_DESC cbd = {};
    cbd.Usage = D3D11_USAGE_DEFAULT;
    cbd.ByteWidth = sizeof(ShadowCB);
    cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    cbd.CPUAccessFlags = 0;

    m_pDevice->CreateBuffer(&cbd, nullptr, &m_pShadowConstantBuffer);

    // Texture 생성
    D3D11_TEXTURE2D_DESC texDesc = {};
    texDesc.Width = SHADOW_WIDTH;
    texDesc.Height = SHADOW_HEIGHT;
    texDesc.MipLevels = 1;
    texDesc.ArraySize = 1;
    texDesc.Usage = D3D11_USAGE_DEFAULT;
    texDesc.Format = DXGI_FORMAT_R32_TYPELESS;
    texDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
    texDesc.SampleDesc.Count = 1;
    texDesc.SampleDesc.Quality = 0;
    HR_T(m_pDevice->CreateTexture2D(&texDesc, NULL, m_pShadowMap.GetAddressOf()));

    // DSV 생성
    D3D11_DEPTH_STENCIL_VIEW_DESC descDSV = {};
    descDSV.Format = DXGI_FORMAT_D32_FLOAT;
    descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    HR_T(m_pDevice->CreateDepthStencilView(m_pShadowMap.Get(), &descDSV, m_pShadowMapDSV.GetAddressOf()));

    // SRV 생성
    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
    srvDesc.Format = DXGI_FORMAT_R32_FLOAT;
    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MipLevels = 1;
    HR_T(m_pDevice->CreateShaderResourceView(m_pShadowMap.Get(), &srvDesc, m_pShadowMapSRV.GetAddressOf()));

    m_ShadowProjection = XMMatrixPerspectiveFovLH(XM_PIDIV4, m_ShadowViewport.Width /
        (float)m_ShadowViewport.Height, m_ShadowProjectionNearFar.x, m_ShadowProjectionNearFar.y);

    m_ShadowLookAt = m_Camera.GetPosition() + m_Camera.GetForward() * m_ShadowForwardDistFromCamera;

    m_ShadowPos = m_ShadowLookAt + (-m_Light.Direction * m_ShadowUpDistFromLookAt);

    m_ShadowView = XMMatrixLookAtLH(m_ShadowPos, m_ShadowLookAt, Vector3(0.0f, 1.0f, 0.0f));
    
    m_pDeviceContext->RSSetViewports(1, &m_ShadowViewport);

    // Create Vertex Buffer
    ID3DBlob* vsBlob;
    CompileShaderFromFile(L"Shader/DepthOnlyPass.hlsl", "main", "vs_5_0", &vsBlob);

    HR_T(m_pDevice->CreateVertexShader(vsBlob->GetBufferPointer(),
        vsBlob->GetBufferSize(), nullptr, m_pShadowVS.GetAddressOf()));

    //D3D11_INPUT_ELEMENT_DESC shadowlayout[] =
    //{
    //    {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
    //    {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
    //};

    //HR_T(m_pDevice->CreateInputLayout(shadowlayout, ARRAYSIZE(shadowlayout), vsBlob->GetBufferPointer(),
    //    vsBlob->GetBufferSize(), &m_pSkyboxInputLayout));

    SAFE_RELEASE(vsBlob);

    // 그려지는 범위 NearZ, FarZ값으로 설정
    m_Projection = XMMatrixPerspectiveFovLH(XM_PIDIV2, m_ClientWidth / (FLOAT)m_ClientHeight, 0.01f, 100.0f);

    m_World = XMMatrixIdentity();
    XMVECTOR Eye = XMVectorSet(0.0f, 4.0f, -10.0f, 0.0f);
    XMVECTOR At = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
    XMVECTOR Up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
    m_View = XMMatrixLookAtLH(Eye, At, Up);
    m_Projection = XMMatrixPerspectiveFovLH(XM_PIDIV2 / 2.0f, m_ClientWidth / (FLOAT)m_ClientHeight, 0.01f, 100.0f);

    m_LightDirsEvaluated = m_InitialLightDirs;

    return true;
}

void TutorialApp::UninitScene()
{
    SAFE_RELEASE(m_pStaticMeshConstantBuffer);

    SAFE_RELEASE(m_pCubeMuseumTextureRV);
    SAFE_RELEASE(m_pCubeDaylightTextureRV);
    SAFE_RELEASE(m_pCubeHanakoTextureRV);

    SAFE_RELEASE(m_pSkyboxConstantBuffer);
    SAFE_RELEASE(m_pShadowConstantBuffer);

    m_ModelLoader.Close();
}