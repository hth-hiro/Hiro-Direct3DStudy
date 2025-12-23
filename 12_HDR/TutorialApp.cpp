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
    UninitImGUI();
    m_uiManager.Shutdown();

    if (m_pDeviceContext)
    {
        m_pDeviceContext->ClearState();
        m_pDeviceContext->Flush();
    }

    m_pRenderTargetView.Reset();
    m_pDepthStencilView.Reset();

    UninitScene();
    UninitD3D();
    CheckDXGIDebug();
}

bool TutorialApp::Initialize(UINT Width, UINT Height)
{
    __super::Initialize(Width, Height);

    m_Time.Initialize();

    if (!InitD3D())
        return false;

    if (!m_uiManager.Initialize(m_hWnd, m_pDevice, m_pDeviceContext, m_pSwapChain))
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

    //m_ImGuiManager.Update();
    //ImGuiIO& io = ImGui::GetIO();

    //static ImVec2 lastMousePosBeforeWarp = ImVec2(-1, 1);

    //// 드래그 중이 아니면 리셋
    //if (!ImGui::IsMouseDown(ImGuiMouseButton_Left))
    //{
    //    lastMousePosBeforeWarp = ImVec2(-1, -1);
    //    return;
    //}

    //// 현재 윈도우 핸들 가져오기
    //HWND hwnd = GetActiveWindow(); // 또는 메인 윈도우 핸들 저장해두기
    //if (!hwnd) return;

    //// 현재 마우스 위치 (스크린 좌표)
    //POINT screenPos;
    //GetCursorPos(&screenPos);

    //// 윈도우 클라이언트 영역 정보
    //RECT clientRect;
    //GetClientRect(hwnd, &clientRect);
    //POINT clientOrigin = { 0, 0 };
    //ClientToScreen(hwnd, &clientOrigin);

    //int clientW = clientRect.right - clientRect.left;
    //int clientH = clientRect.bottom - clientRect.top;

    //// 클라이언트 영역 기준으로 변환
    //int localX = screenPos.x - clientOrigin.x;
    //int localY = screenPos.y - clientOrigin.y;

    //// 워프 직전 델타 저장
    //ImVec2 currentDelta = io.MouseDelta;

    //// 경계 체크 및 워프 (여유 공간 10px)
    //const int margin = 10;
    //bool wrapped = false;
    //POINT newScreenPos = screenPos;

    //if (localX <= margin)
    //{
    //    newScreenPos.x = clientOrigin.x + clientW - margin - 1;
    //    wrapped = true;
    //}
    //else if (localX >= clientW - margin)
    //{
    //    newScreenPos.x = clientOrigin.x + margin + 1;
    //    wrapped = true;
    //}

    //if (localY <= margin)
    //{
    //    newScreenPos.y = clientOrigin.y + clientH - margin - 1;
    //    wrapped = true;
    //}
    //else if (localY >= clientH - margin)
    //{
    //    newScreenPos.y = clientOrigin.y + margin + 1;
    //    wrapped = true;
    //}

    //if (wrapped)
    //{
    //    // 워프 직전 위치 저장
    //    lastMousePosBeforeWarp = io.MousePos;

    //    // 커서 워프
    //    SetCursorPos(newScreenPos.x, newScreenPos.y);

    //    // ImGui에게 새 위치 알리기 (클라이언트 좌표로)
    //    io.MousePos = ImVec2(
    //        (float)(newScreenPos.x - clientOrigin.x),
    //        (float)(newScreenPos.y - clientOrigin.y)
    //    );

    //    // 중요: Delta는 유지! 워프는 화면상 위치만 바꿀 뿐
    //    // 실제 마우스 이동량은 그대로 전달되어야 함
    //    io.MouseDelta = currentDelta;

    //    // WantSetMousePos로 ImGui에게 위치 변경 알림
    //    io.WantSetMousePos = true;
    //}
    //else if (lastMousePosBeforeWarp.x >= 0)
    //{
    //    // 워프 직후 첫 프레임
    //    // 이때 Delta가 비정상적으로 클 수 있으므로 보정
    //    ImVec2 expectedDelta = ImVec2(
    //        io.MousePos.x - lastMousePosBeforeWarp.x,
    //        io.MousePos.y - lastMousePosBeforeWarp.y
    //    );

    //    // Delta가 비정상적으로 크면 (화면 반대편으로 점프) 무시
    //    float deltaLen = sqrtf(expectedDelta.x * expectedDelta.x +
    //        expectedDelta.y * expectedDelta.y);
    //    if (deltaLen > 100.0f) // threshold
    //    {
    //        io.MouseDelta = ImVec2(0, 0);
    //    }

    //    lastMousePosBeforeWarp = ImVec2(-1, -1);
    //}

    // Light
    m_InitialLightDirs = { lightDir.x, lightDir.y, lightDir.z, 0.0f };
    m_LightDirsEvaluated = m_InitialLightDirs;
    m_Light.Direction = { lightDir.x, lightDir.y, lightDir.z };

    m_AmbientColor = { ambientLight };
    m_DiffuseColor = { diffuseLight };
    m_SpecularColor = { specularLight };

    // Camera
    m_cameraPos = { m_Camera.GetPosition().x, m_Camera.GetPosition().y, m_Camera.GetPosition().z, 1 };
    float nearZ = depth.x;
    float farZ = depth.y;
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
    m_Projection = XMMatrixPerspectiveFovLH(XM_PIDIV4 / FOV, m_ClientWidth / (FLOAT)m_ClientHeight, nearZ, finalFarZ);
    m_Camera.GetViewMatrix(m_View);

    //if (viewChanger)
    //{
    //    //m_pCubeTextureRV = m_pCubeDaylightTextureRV;
    //    m_pCubeTextureRV = m_pCubeHanakoTextureRV;
    //}
    //else
    //{
    //    m_pCubeTextureRV = m_pCubeMuseumTextureRV;
    //}

    m_ShadowProjection = XMMatrixPerspectiveFovLH(XMConvertToRadians(10), m_ShadowViewport.Width /
        (float)m_ShadowViewport.Height, m_ShadowProjectionNearFar.x, m_ShadowProjectionNearFar.y);
    m_ShadowLookAt = Vector3(0, 0, 0);
    m_ShadowPos = m_ShadowLookAt + (-m_Light.Direction * m_ShadowUpDistFromLookAt);
    m_ShadowView = XMMatrixLookAtLH(m_ShadowPos, m_ShadowLookAt, Vector3(0.0f, 1.0f, 0.0f));
    m_pDeviceContext->RSSetViewports(1, &m_ShadowViewport);

    // UI update
    m_title.uiText->SetActive(m_title.active);
    m_title.uiText->SetText(L"Text");
    m_title.uiText->SetFont(L"Gulim");
    m_title.uiText->SetFontSize(m_title.fontSize);
    m_title.uiText->SetColor(m_title.textColor);
    m_title.uiText->SetRect(m_title.position.x, m_title.position.y, m_title.textBox.x, m_title.textBox.y);
}

void TutorialApp::Render()
{
    Vector3 ObjectPos = { object1.transform.GetPosition().x, object1.transform.GetPosition().y , object1.transform.GetPosition().z };

    // 1. Clear
    float color[4] = { 0.0f, 0.7f, 0.7f, 1.0f };
    float blendFactor[4] = { 0.f, 0.f, 0.f, 0.f }; // 일반적으로 0,0,0,0
    UINT sampleMask = 0xffffffff; // 모든 샘플 사용
    ID3D11ShaderResourceView* nullSRV[1] = { nullptr };

    m_pDeviceContext->OMSetBlendState(m_pBlendState.Get(), blendFactor, sampleMask);

    // ShadowPass ================================================================================================================
    m_pDeviceContext->IASetInputLayout(m_pInputLayout.Get());
    m_pDeviceContext->RSSetViewports(1, &m_ShadowViewport);
    m_pDeviceContext->OMSetRenderTargets(0, nullptr, m_pShadowMapDSV.Get());
    m_pDeviceContext->ClearDepthStencilView(m_pShadowMapDSV.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);
    m_pDeviceContext->VSSetShader(m_pShadowVS.Get(), nullptr, 0);
    m_pDeviceContext->PSSetShader(nullptr, nullptr, 0);

    // 상수 버퍼 업데이트
    ConstantBuffer cb;
    cb.mWorld = XMMatrixTranspose(
        XMMatrixScaling(object1.transform.GetScale().x,
            object1.transform.GetScale().y,
            object1.transform.GetScale().z) *
        XMMatrixRotationRollPitchYaw(object1.transform.GetRotation().x,
            object1.transform.GetRotation().y,
            object1.transform.GetRotation().z) *
        XMMatrixTranslation(ObjectPos.x, ObjectPos.y, ObjectPos.z)
    );
    //cb.mWorld = XMMatrixTranspose(m_StaticMesh.m_World) *
    //    XMMatrixTranspose(XMMatrixScaling(object1.transform.GetScale().x, object1.transform.GetScale().y, object1.transform.GetScale().z))
    //    * XMMatrixTranspose(XMMatrixRotationRollPitchYaw(object1.transform.GetRotation().x, object1.transform.GetRotation().y, object1.transform.GetRotation().z))
    //    * XMMatrixTranspose(XMMatrixTranslation(ObjectPos.x, ObjectPos.y, ObjectPos.z))
    //    ;
    cb.ShadowView = XMMatrixTranspose(m_ShadowView);
    cb.ShadowProjection = XMMatrixTranspose(m_ShadowProjection);
    m_pDeviceContext->VSSetConstantBuffers(0, 1, &m_pShadowConstantBuffer);
    m_pDeviceContext->UpdateSubresource(m_pShadowConstantBuffer, 0, nullptr, &cb, 0, 0);

    for (auto& section : m_StaticMesh.m_StaticMeshSection)
    {
        // Vertex/Index Buffer 바인딩
        UINT stride = sizeof(Vertex);
        UINT offset = 0;
        m_pDeviceContext->IASetVertexBuffers(0, 1, section.VertexBuffer.GetAddressOf(), &stride, &offset);
        m_pDeviceContext->IASetIndexBuffer(section.IndexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0);
        m_pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

        // Shadow Pass에서는 PS 없이 Depth만 기록하므로 PS 텍스처 바인딩 불필요

        // Draw
        m_pDeviceContext->DrawIndexed(static_cast<UINT>(section.Indices.size()), 0, 0);
    }

    //ID3D11RenderTargetView* rtv = m_pRenderTargetView.Get(); // 내부 포인터
    //m_pDeviceContext->OMSetRenderTargets(1, &rtv, m_pDepthStencilView.Get());
    //m_pDeviceContext->ClearRenderTargetView(rtv, color);

    // HDR
    m_pDeviceContext->OMSetRenderTargets(1, m_sceneHDRRTV.GetAddressOf(), m_pDepthStencilView.Get());
    m_pDeviceContext->ClearRenderTargetView(m_sceneHDRRTV.Get(), color);
    m_pDeviceContext->ClearDepthStencilView(m_pDepthStencilView.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);
    m_pDeviceContext->RSSetViewports(1, &m_MainViewport);
    m_pDeviceContext->OMSetDepthStencilState(nullptr, 0);

    float hdrClear[4] = { 4.0f, 0.0f, 4.0f, 1.0f };
    m_pDeviceContext->ClearRenderTargetView(m_sceneHDRRTV.Get(), hdrClear);
    m_pDeviceContext->ClearDepthStencilView(m_pDepthStencilView.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);

    m_pDeviceContext->RSSetState(m_pRasterStateNoCull.Get());
    m_pDeviceContext->OMSetBlendState(nullptr, nullptr, 0xffffffff); // 블렌딩 OFF

    // 2. 스카이박스 렌더 =================================================================================================================
    m_pDeviceContext->OMSetDepthStencilState(m_pSkyboxDepthStencilState, 0);
    m_pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    m_pDeviceContext->IASetVertexBuffers(0, 1, &m_pSkyboxVertexBuffer, &m_SkyboxVertexBufferStride, &m_SkyboxVertexBufferOffset);
    m_pDeviceContext->IASetIndexBuffer(m_pSkyboxIndexBuffer, DXGI_FORMAT_R16_UINT, 0);
    m_pDeviceContext->IASetInputLayout(m_pSkyboxInputLayout.Get());
    m_pDeviceContext->VSSetShader(m_pSkyboxVertexShader.Get(), nullptr, 0);
    m_pDeviceContext->PSSetShader(m_pSkyboxPixelShader.Get(), nullptr, 0);

    // 상수 버퍼 업데이트
    SkyBoxCB cbSky;
    cbSky.mView = XMMatrixTranspose(XMMatrixScaling(10, 10, 10) * m_Camera.GetViewMatrixNoTranslation(m_View));
    cbSky.mProjection = XMMatrixTranspose(m_Projection);
    m_pDeviceContext->VSSetConstantBuffers(1, 1, &m_pSkyboxConstantBuffer);
    m_pDeviceContext->UpdateSubresource(m_pSkyboxConstantBuffer, 0, nullptr, &cbSky, 0, 0);

    m_pDeviceContext->PSSetShaderResources(0, 1, currentEnv->SkyBox.GetAddressOf());
    m_pDeviceContext->PSSetSamplers(2, 1, m_pSamplerLinear.GetAddressOf());
    m_pDeviceContext->DrawIndexed(m_nSkyboxIndices, 0, 0);
    // 원래 상태 복원
    m_pDeviceContext->OMSetDepthStencilState(nullptr, 0);

    // 3. 일반 오브젝트 렌더 ===========================================================================================
    m_pDeviceContext->IASetInputLayout(m_pInputLayout.Get());
    m_pDeviceContext->VSSetShader(m_pVertexShader.Get(), nullptr, 0);
    m_pDeviceContext->PSSetShader(m_pPixelShader.Get(), nullptr, 0);
    m_pDeviceContext->RSSetState(m_pRasterStateNoCull.Get());
    // Sampler
    m_pDeviceContext->PSSetSamplers(0, 1, m_pSamplerLinear.GetAddressOf());
    // ShadowMapSRV
    m_pDeviceContext->PSSetShaderResources(7, 1, m_pShadowMapSRV.GetAddressOf());

    // IBL 적용 여부 분기, 매핑 여부 결정
    if (useIBL)
    {
        m_pDeviceContext->PSSetShaderResources(10, 1, currentEnv->Diffuse.GetAddressOf());
        m_pDeviceContext->PSSetShaderResources(11, 1, currentEnv->Specular.GetAddressOf());
        m_pDeviceContext->PSSetShaderResources(12, 1, currentEnv->BRDF_LUT.GetAddressOf());
    }
    else
    {
        m_pDeviceContext->PSSetShaderResources(0, 1, nullSRV);
        m_pDeviceContext->PSSetShaderResources(10, 1, nullSRV);
        m_pDeviceContext->PSSetShaderResources(11, 1, nullSRV);
        m_pDeviceContext->PSSetShaderResources(12, 1, nullSRV);
    }

    // Object - StaticMesh
    for (auto& section : m_StaticMesh.m_StaticMeshSection)
    {
        int matIdx = section.materialIndex;
        if (matIdx < 0 || matIdx >= m_StaticMesh.m_Materials.size())
            continue;

        Material& mat = m_StaticMesh.m_Materials[matIdx];

        cb.mWorld = XMMatrixTranspose(
            XMMatrixScaling(object1.transform.GetScale().x,
                object1.transform.GetScale().y,
                object1.transform.GetScale().z) *
            XMMatrixRotationRollPitchYaw(object1.transform.GetRotation().x,
                object1.transform.GetRotation().y,
                object1.transform.GetRotation().z) *
            XMMatrixTranslation(ObjectPos.x, ObjectPos.y, ObjectPos.z)
        );

        //cb.mWorld = XMMatrixTranspose(XMMatrixScaling(object1.transform.GetScale().x, object1.transform.GetScale().y, object1.transform.GetScale().z))
        //    * XMMatrixTranspose(XMMatrixRotationRollPitchYaw(object1.transform.GetRotation().x, object1.transform.GetRotation().y, object1.transform.GetRotation().z))
        //    * XMMatrixTranspose(XMMatrixTranslation(ObjectPos.x, ObjectPos.y, ObjectPos.z))
        //    * XMMatrixTranspose(m_StaticMesh.m_World);
        cb.mView = XMMatrixTranspose(m_View);
        cb.mProjection = XMMatrixTranspose(m_Projection);
        cb.vLightDir = m_LightDirsEvaluated;

        // 머티리얼 정보
        cb.vMaterialAmbient = object1.ambient;
        cb.vMaterialDiffuse = object1.diffuse;
        cb.vMaterialSpecular = object1.specular;
        cb.vShininess = { object1.shininess };

        cb.vAmbientColor = m_AmbientColor;
        cb.vDiffuseColor = m_DiffuseColor;
        cb.vSpecularColor = m_SpecularColor;
        cb.cameraPos = m_cameraPos;
        cb.UseLighting = useLighting;

        // 텍스처 관련
        cb.hasTexture = mat.hasTexture ? 1 : 0;
        cb.solidColor = mat.hasTexture ? XMFLOAT4(1, 1, 1, 1) : mat.solidColor;
        cb.hasNormalMap = mat.hasNormalMap ? 1 : 0;
        cb.hasSpecularMap = mat.hasSpecularMap ? 1 : 0;
        cb.hasEmissiveMap = mat.hasEmissiveMap ? 1 : 0;

        cb.hasMetallicMap = mat.hasMetallicMap ? 1 : 0;
        cb.hasRoughnessMap = mat.hasRoughnessMap ? 1 : 0;

        // PBR
        cb.metallic = object1.metallic;
        cb.roughness = object1.roughness;
        cb.albedo = object1.albedo;
        cb.gamma = object1.gamma;
        cb.ao = ambientOcclusion;
        cb.UseTexture = useTexture;
        cb.UseCustomAlbedo = useCustomAlbedo;

        // IBL
        cb.UseIBL = useIBL;

        cb.ShadowView = XMMatrixTranspose(m_ShadowView);
        cb.ShadowProjection = XMMatrixTranspose(m_ShadowProjection);

        // 상수버퍼 업데이트
        m_pDeviceContext->VSSetConstantBuffers(0, 1, &m_pStaticMeshConstantBuffer);
        m_pDeviceContext->PSSetConstantBuffers(0, 1, &m_pStaticMeshConstantBuffer);
        m_pDeviceContext->UpdateSubresource(m_pStaticMeshConstantBuffer, 0, nullptr, &cb, 0, 0);

        // Vertex/Index Buffer
        UINT stride = sizeof(Vertex);
        UINT offset = 0;
        m_pDeviceContext->IASetVertexBuffers(0, 1, section.VertexBuffer.GetAddressOf(), &stride, &offset);
        m_pDeviceContext->IASetIndexBuffer(section.IndexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0);
        m_pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

        // PS 텍스처 바인딩
        for (int i = 1; i <= 6; i++)
            m_pDeviceContext->PSSetShaderResources(i, 1, nullSRV);

        if (mat.hasTexture && mat.diffuseSRV)
            m_pDeviceContext->PSSetShaderResources(1, 1, &mat.diffuseSRV);
        if (mat.hasNormalMap && mat.normalSRV)
            m_pDeviceContext->PSSetShaderResources(2, 1, &mat.normalSRV);
        if (mat.hasSpecularMap && mat.specularSRV)
            m_pDeviceContext->PSSetShaderResources(3, 1, &mat.specularSRV);
        if (mat.hasEmissiveMap && mat.emissiveSRV)
            m_pDeviceContext->PSSetShaderResources(4, 1, &mat.emissiveSRV);
        if (mat.hasMetallicMap && mat.metallicSRV)
            m_pDeviceContext->PSSetShaderResources(5, 1, &mat.metallicSRV);
        if (mat.hasRoughnessMap && mat.roughnessSRV)
            m_pDeviceContext->PSSetShaderResources(6, 1, &mat.roughnessSRV);

        // Draw 호출
        m_pDeviceContext->DrawIndexed(static_cast<UINT>(section.Indices.size()), 0, 0);
    }

    //for (auto& section : ground.m_StaticMeshSection)
    //{
    //    int matIdx = section.materialIndex;
    //    if (matIdx < 0 || matIdx >= ground.m_Materials.size())
    //        continue;

    //    Material& mat = ground.m_Materials[matIdx];

    //    cb.mWorld = XMMatrixTranspose(XMMatrixScaling(0.01, 0.01, 0.01)) * XMMatrixTranspose(XMMatrixTranslation(0, -100, 0)) * XMMatrixTranspose(ground.m_World);
    //    cb.mView = XMMatrixTranspose(m_View);
    //    cb.mProjection = XMMatrixTranspose(m_Projection);
    //    cb.vLightDir = m_LightDirsEvaluated;

    //    // 머티리얼 정보
    //    cb.vMaterialAmbient = mat.ambient;
    //    cb.vMaterialDiffuse = mat.diffuse;
    //    cb.vMaterialSpecular = mat.specular;
    //    cb.vShininess = mat.shininess;

    //    cb.vAmbientColor = m_AmbientColor;
    //    cb.vDiffuseColor = m_DiffuseColor;
    //    cb.vSpecularColor = m_SpecularColor;
    //    cb.cameraPos = m_cameraPos;
    //    cb.UseLighting = useLighting;

    //    // PBR
    //    //cb.metalness = m_Metalness;
    //    //cb.roughness = m_Roughness;
    //    //cb.albedo = m_Albedo;

    //    // 땅은 그냥 고정값으로 들어가게 설정
    //    cb.metallic = 0.f;
    //    cb.roughness = 0.5f;
    //    cb.albedo = Vector4(0, 0, 0, 1);

    //    // 텍스처 관련
    //    cb.hasTexture = mat.hasTexture ? 1 : 0;
    //    cb.solidColor = mat.hasTexture ? XMFLOAT4(1, 1, 1, 1) : mat.solidColor;
    //    cb.hasNormalMap = mat.hasNormalMap ? 1 : 0;
    //    cb.hasSpecularMap = mat.hasSpecularMap ? 1 : 0;
    //    cb.hasEmissiveMap = mat.hasEmissiveMap ? 1 : 0;

    //    cb.hasMetallicMap = mat.hasMetallicMap ? 1 : 0;
    //    cb.hasRoughnessMap = mat.hasRoughnessMap ? 1 : 0;

    //    cb.ShadowView = XMMatrixTranspose(m_ShadowView);
    //    cb.ShadowProjection = XMMatrixTranspose(m_ShadowProjection);

    //    // 상수버퍼 업데이트
    //    m_pDeviceContext->VSSetConstantBuffers(0, 1, &m_pStaticMeshConstantBuffer);
    //    m_pDeviceContext->PSSetConstantBuffers(0, 1, &m_pStaticMeshConstantBuffer);
    //    m_pDeviceContext->UpdateSubresource(m_pStaticMeshConstantBuffer, 0, nullptr, &cb, 0, 0);

    //    // Vertex/Index Buffer
    //    UINT stride = sizeof(Vertex);
    //    UINT offset = 0;
    //    m_pDeviceContext->IASetVertexBuffers(0, 1, &section.VertexBuffer, &stride, &offset);
    //    m_pDeviceContext->IASetIndexBuffer(section.IndexBuffer, DXGI_FORMAT_R16_UINT, 0);
    //    m_pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    //    // PS 텍스처 바인딩
    //    ID3D11ShaderResourceView* nullSRV[1] = { nullptr };
    //    for (int i = 0; i <= 4; i++)
    //        m_pDeviceContext->PSSetShaderResources(i, 1, nullSRV);

    //    if (mat.hasTexture && mat.diffuseSRV)
    //        m_pDeviceContext->PSSetShaderResources(0, 1, &mat.diffuseSRV);
    //    if (mat.hasNormalMap && mat.normalSRV)
    //        m_pDeviceContext->PSSetShaderResources(2, 1, &mat.normalSRV);
    //    if (mat.hasSpecularMap && mat.specularSRV)
    //        m_pDeviceContext->PSSetShaderResources(3, 1, &mat.specularSRV);
    //    if (mat.hasEmissiveMap && mat.emissiveSRV)
    //        m_pDeviceContext->PSSetShaderResources(4, 1, &mat.emissiveSRV);

    //    // Draw 호출
    //    m_pDeviceContext->DrawIndexed(static_cast<UINT>(section.Indices.size()), 0, 0);
    //}

    m_pDeviceContext->PSSetShaderResources(7, 1, nullSRV);

    // Tone Mapping
    m_pDeviceContext->OMSetDepthStencilState(nullptr, 0);
    m_pDeviceContext->OMSetRenderTargets(1, m_pRenderTargetView.GetAddressOf(), nullptr);
    m_pDeviceContext->RSSetViewports(1, &m_MainViewport);

    // Pipeline
    m_pDeviceContext->IASetInputLayout(m_toneMapInputLayout.Get());
    m_pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    UINT fsStride = sizeof(FullScreenVertex);
    UINT fsOffset = 0;
    ID3D11Buffer* fsVB = m_fullscreenVB.Get();
    m_pDeviceContext->IASetVertexBuffers(0, 1, &fsVB, &fsStride, &fsOffset);

    m_pDeviceContext->VSSetShader(m_toneMapVertexShader.Get(), nullptr, 0);
    m_pDeviceContext->PSSetShader(m_toneMapPixelShader.Get(), nullptr, 0);

    ToneMapCB toneCB = {};
    toneCB.Exposure = m_exposure;
    toneCB.Gamma = m_gamma;

    m_pDeviceContext->UpdateSubresource(m_toneMapConstantBuffer.Get(), 0, nullptr, &toneCB, 0, 0);
    m_pDeviceContext->PSSetConstantBuffers(5, 1, m_toneMapConstantBuffer.GetAddressOf());

    m_pDeviceContext->PSSetShaderResources(13, 1, m_sceneHDRSRV.GetAddressOf());

    m_pDeviceContext->PSSetSamplers(2, 1, m_pSamplerLinear.GetAddressOf());

    // 인덱스 없이 VB에 6개를 넣음
    m_pDeviceContext->Draw(6, 0);

    // 텍스처 해제
    ID3D11ShaderResourceView* nullSrv[1] = { nullptr };
    m_pDeviceContext->PSSetShaderResources(13, 1, nullSrv);

    // 4. GUI 렌더 ======================================================================================
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    //m_ImGuiManager.DrawObjectUI();
    ImGuiIO& io = ImGui::GetIO(); (void)io;

    ImGui::SetNextWindowPos(ImVec2(30, 30), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(400, 716), ImGuiCond_FirstUseEver);
    ImGui::Begin(u8"컨트롤러");
    ImGui::SeparatorText(u8"오브젝트 조절");
    ImGui::DragFloat3(u8" 오브젝트 크기", &object1.transform.scale.x, 1.0f, 100.0f);
    ImGui::SliderFloat3(u8" 오브젝트 위치", &object1.transform.position.x, -100.0f, 100.0f, "%.1f");
    ImGui::DragFloat3(u8" 오브젝트 회전", &object1.transform.rotation.x, 0.1f);
    if (ImGui::Button(u8" 오브젝트 초기화")) object1.PosReset();

    ImGui::SeparatorText(u8"빛 조절");
    //ImGui::ColorEdit4(u8" Ambient 색상", &ambientLight.x);
    //ImGui::ColorEdit4(u8" Diffuse 색상", &diffuseLight.x);
    //ImGui::ColorEdit4(u8" Specular 색상", &specularLight.x);
    ImGui::DragFloat3(u8" 방향", &lightDir.x, 0.1, -1.0f, 1.0f, "%.1f");

    // 안전하게 x나 z가 0이 되어도 그림자 생성
    if (fabs(lightDir.x) < 0.1f && fabs(lightDir.z) < 0.1f)
    {
        lightDir.x = (lightDir.x >= 0.0f ? 1.0f : -1.0f) * 0.01f;
        lightDir.z = (lightDir.z >= 0.0f ? 1.0f : -1.0f) * 0.01f;
    }

    ImGui::Checkbox("Use Lighting", &useLighting);

    if (ImGui::Button(u8" 빛 초기화"))LightReset();

    ImGui::Text("");
    ImGui::SeparatorText(u8" 카메라 조절");
    ImGui::SliderFloat(u8" 시야범위", &FOV, 0.6f, 100.0f);
    ImGui::DragFloat2(u8" 렌더 최소/최대 거리", &depth.x, 0.1f, 1.0f, 10000.0f, "%.1f");
    if (depth.x > depth.y) depth.y = depth.x;

    if (ImGui::Button(u8"카메라 초기화"))
    {
        FOV = 1.0f;

        depth.x = 1.0f;
        depth.y = 10000.0f;
    }

    ImGui::Text("");

    ImGui::Checkbox(u8"IBL 사용", &useIBL);

    ImGui::SeparatorText(u8" 배경 선택");
    static int currentBackground = 0;
    const char* viewChanger[] = { u8"샘플", u8"하늘", u8"하나코", u8"실내", u8"밤", u8"거리" };

    if (ImGui::Combo(u8"배경", &currentBackground, viewChanger, IM_ARRAYSIZE(viewChanger)))
    {
        switch (currentBackground)
        {
        case 0:
            currentEnv = &Env_BakerSample;
            break;
        case 1:
            currentEnv = &Env_DayLight;
            break;
        case 2:
            currentEnv = &Env_Hanako;
            break;
        case 3:
            currentEnv = &Env_Museum;
            break;
        case 4:
            currentEnv = &Env_Night;
            break;
        case 5:
            currentEnv = &Env_Street;
            break;
        }
    }

    ImGui::Text("");
    ImGui::Separator();
    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);

    //ImGui::DragFloat2(u8"디버그", &m_ShadowProjectionNearFar.x, 0.1, 0.01, 50000000.f);
    //ImGui::DragFloat(u8"디버그2", &m_ShadowUpDistFromLookAt);

    ImGui::End();

    //m_ImGuiManager.DrawShadowSRV(m_pShadowMapSRV.Get(), (float)SHADOW_WIDTH, (float)SHADOW_HEIGHT);
    ImGui::SetNextWindowPos(ImVec2(440, 30), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(272, 298), ImGuiCond_FirstUseEver);
    ImGui::Begin(u8"그림자 맵");
    ImGui::Image((ImTextureID)m_pShadowMapSRV.Get(), ImVec2(256, 256));
    ImGui::End();

    ImGui::SetNextWindowPos(ImVec2(440, 340), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(408, 311), ImGuiCond_FirstUseEver);
    ImGui::Begin(u8"PBR");
    // BaseColor
    //ImGui::ColorEdit4(u8" 오브젝트 Material Ambient", &object1.ambient.x);
    //ImGui::ColorEdit4(u8" 오브젝트 Material Diffuse", &object1.diffuse.x);
    //ImGui::ColorEdit4(u8" 오브젝트 Material Specular", &object1.specular.x);
    //ImGui::SliderFloat(u8" 오브젝트 광택지수", &object1.shininess, 200.0f, 20000.0f);

    ImGui::ColorEdit4(u8"알베도", &object1.albedo.x);
    ImGui::SliderFloat(u8"Metallic", &object1.metallic, 0.0f, 1.0f);
    ImGui::SliderFloat(u8"Roughness", &object1.roughness, 0.0f, 1.0f);
    //ImGui::SliderFloat(u8"Gamma", &object1.gamma, 1.f, 3.0f);

    ImGui::Checkbox(u8"텍스처 적용", &useTexture);
    ImGui::Checkbox(u8"PBR 수동 조작", &useCustomAlbedo);
    ImGui::DragFloat(u8"앰비언트 오클루전", &ambientOcclusion, 0.01f, 0.0f, 1.0f);
    if (ImGui::Button(u8" 초기화")) { object1.Reset(); ambientOcclusion = 1.0f; }
    ImGui::Text("");
    ImGui::End();

    ImGui::Begin(u8"텍스트 관련 설정");
    ImGui::Checkbox(u8"텍스트 활성화", &m_title.active);
    ImGui::ColorEdit4(u8"텍스트 색", &m_title.textColor.x);
    ImGui::DragFloat(u8"텍스트 크기", &m_title.fontSize, 0.1f, 1.0f, 500.0f, "%.1f");
    ImGui::DragFloat2(u8"텍스트 위치", &m_title.position.x, 1.f, 0.0f, m_ClientWidth, "%.0f");
    ImGui::DragFloat2(u8"텍스트박스 크기", &m_title.textBox.x, 1.0, 1.0f, 1000.0f);
    
    if (ImGui::Button(u8" 초기화")) { m_title.Reset(); }
    ImGui::End();

    ImGui::Begin("HDR");
    //ImGui::Image((ImTextureID)m_sceneHDRSRV.Get(), ImVec2(256, 256));
    ImGui::DragFloat(u8"카메라 노출", &m_exposure, 0.001f);
    ImGui::DragFloat(u8"카메라 감마", &m_gamma, 0.001f, 0.0f, 10.0f);
    if (ImGui::Button(u8"초기화")) { m_exposure = 0.0f; m_gamma = 1.0f; }
    ImGui::End();

    // etc... ImGui...

    //m_ImGuiManager.Render();
    ImGui::Render();
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

    m_uiManager.Render();

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

    hr = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, /*D3D11_CREATE_DEVICE_DEBUG |*/ D3D11_CREATE_DEVICE_BGRA_SUPPORT, NULL, NULL,
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

    SAFE_RELEASE(depthStencilBuffer);

    // 깊이 테스트 - 오브젝트 렌더 시 DepthEnable = true 상태로 쓰고 있음
    D3D11_DEPTH_STENCIL_DESC dsDesc = {};
    dsDesc.DepthEnable = true;
    dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    dsDesc.DepthFunc = D3D11_COMPARISON_LESS;

    ID3D11DepthStencilState* depthStencilState;
    m_pDevice->CreateDepthStencilState(&dsDesc, &depthStencilState);
    m_pDeviceContext->OMSetDepthStencilState(depthStencilState, 0);

    SAFE_RELEASE(depthStencilState);

    D3D11_DEPTH_STENCIL_DESC skyDesc = {};
    skyDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
    skyDesc.DepthEnable = true;
    skyDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
    skyDesc.StencilEnable = false;
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
    SAFE_RELEASE(m_pSwapChain);
    SAFE_RELEASE(m_pDeviceContext);
    SAFE_RELEASE(m_pDevice);
}

bool TutorialApp::InitImGUI()
{
    LightReset();

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    // GUI의 테마를 결정한다.
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsLight();
    //ImGui::StyleColorsClassic();

    ImGui_ImplWin32_Init(GameApp::m_hWnd);
    ImGui_ImplDX11_Init(m_pDevice, m_pDeviceContext);

    ImGuiIO& io = ImGui::GetIO(); (void)io;

    io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\Malgun.ttf", 20.0f, NULL, io.Fonts->GetGlyphRangesKorean());
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
    return true;
}

void TutorialApp::UninitImGUI()
{
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
}

bool TutorialApp::InitScene()
{
    // UI - Text
    m_title.uiText = new UIText();
    m_uiManager.AddUI(m_title.uiText);

    HRESULT hr = 0;
    ID3D10Blob* errorMessage = nullptr;

    //m_SkeletalModelLoader.Load(m_pDevice, m_pDeviceContext, "../Resource/Vampire_SkinningTest.fbx", "model");
    //m_SkeletalModelLoader.Load(m_pDevice, m_pDeviceContext, "../Resource/SkinningTest.fbx", "SkinningTest");

    FullScreenVertex v[6] =
    {
        { {-1.0f, -1.0f, 0.0f}, {0.0f, 1.0f} },
        { {-1.0f,  1.0f, 0.0f}, {0.0f, 0.0f} },
        { { 1.0f,  1.0f, 0.0f}, {1.0f, 0.0f} },

        { {-1.0f, -1.0f, 0.0f}, {0.0f, 1.0f} },
        { { 1.0f,  1.0f, 0.0f}, {1.0f, 0.0f} },
        { { 1.0f, -1.0f, 0.0f}, {1.0f, 1.0f} },
    };

    D3D11_BUFFER_DESC bdToneMap = {};
    bdToneMap.Usage = D3D11_USAGE_DEFAULT;
    bdToneMap.ByteWidth = sizeof(v);
    bdToneMap.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    D3D11_SUBRESOURCE_DATA init = {};
    init.pSysMem = v;

    HR_T(m_pDevice->CreateBuffer(&bdToneMap, &init, m_fullscreenVB.GetAddressOf()));

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
    CompileShaderFromFile(L"Shader/BasicColor_VS.hlsl", "main", "vs_4_0", &vertexShaderBuffer);

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
    CompileShaderFromFile(L"Shader/BasicColor_PS.hlsl", "main", "ps_5_0", &pixelShaderBuffer);

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



    //m_StaticMesh.ReadFile(m_pDevice, "../Resource/Appearance Miku/Appearance Miku.fbx");
    m_StaticMesh.ReadFile(m_pDevice, "../Resource/Warrior/char.fbx");
    //m_StaticMesh.ReadFile(m_pDevice, "../Resource/sphere.fbx");
    //m_StaticMesh.ReadFile(m_pDevice, "../Resource/Ena_basic/ena_basic.fbx");
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
    CompileShaderFromFile(L"Shader/BoneModel_VS.hlsl", "main", "vs_5_0", &boneShader);

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
    CompileShaderFromFile(L"Shader/Skybox_VS.hlsl", "main", "vs_4_0", &vsBlobSkybox);

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
    CompileShaderFromFile(L"Shader/Skybox_PS.hlsl", "main", "ps_4_0", &psBlobSkybox);
    HR_T(m_pDevice->CreatePixelShader(psBlobSkybox->GetBufferPointer(),
        psBlobSkybox->GetBufferSize(), NULL, &m_pSkyboxPixelShader));

    SAFE_RELEASE(psBlobSkybox);

    skyVBDesc.Usage = D3D11_USAGE_DEFAULT;
    skyVBDesc.ByteWidth = sizeof(ConstantBuffer);
    skyVBDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    skyVBDesc.CPUAccessFlags = 0;
    HR_T(m_pDevice->CreateBuffer(&skyVBDesc, nullptr, &m_pSkyboxConstantBuffer));

    // Skybox 파일 로드
    //HR_T(CreateDDSTextureFromFile(m_pDevice, L"../Resource/Environment/cubemap.dds", nullptr, &m_pCubeMuseumTextureRV));
    //HR_T(CreateDDSTextureFromFile(m_pDevice, L"../Resource/Environment/Daylight.dds", nullptr, &m_pCubeDaylightTextureRV));
    //HR_T(CreateDDSTextureFromFile(m_pDevice, L"../Resource/Environment/Hanako.dds", nullptr, &m_pCubeHanakoTextureRV));

    //m_pCubeTextureRV = m_pCubeMuseumTextureRV;

    // IBL

    // Texture 파일 로드
    // Baker_Sample
    HR_T(CreateDDSTextureFromFile(m_pDevice, L"../Resource/Environment/BakerSample/BakerSampleEnvHDR.dds", nullptr, Env_BakerSample.SkyBox.GetAddressOf()));
    HR_T(CreateDDSTextureFromFile(m_pDevice, L"../Resource/Environment/BakerSample/BakerSampleDiffuseHDR.dds", nullptr, Env_BakerSample.Diffuse.GetAddressOf()));
    HR_T(CreateDDSTextureFromFile(m_pDevice, L"../Resource/Environment/BakerSample/BakerSampleSpecularHDR.dds", nullptr, Env_BakerSample.Specular.GetAddressOf()));
    HR_T(CreateDDSTextureFromFile(m_pDevice, L"../Resource/Environment/BakerSample/BakerSampleBrdf.dds", nullptr, Env_BakerSample.BRDF_LUT.GetAddressOf()));

    // DayLight
    HR_T(CreateDDSTextureFromFile(m_pDevice, L"../Resource/Environment/DayLight/Daylight2EnvHDR.dds", nullptr, Env_DayLight.SkyBox.GetAddressOf()));
    HR_T(CreateDDSTextureFromFile(m_pDevice, L"../Resource/Environment/DayLight/Daylight2DiffuseHDR.dds", nullptr, Env_DayLight.Diffuse.GetAddressOf()));
    HR_T(CreateDDSTextureFromFile(m_pDevice, L"../Resource/Environment/DayLight/Daylight2SpecularHDR.dds", nullptr, Env_DayLight.Specular.GetAddressOf()));
    HR_T(CreateDDSTextureFromFile(m_pDevice, L"../Resource/Environment/DayLight/Daylight2Brdf.dds", nullptr, Env_DayLight.BRDF_LUT.GetAddressOf()));

    // Hanako
    HR_T(CreateDDSTextureFromFile(m_pDevice, L"../Resource/Environment/Hanako/HanakoEnvHDR.dds", nullptr, Env_Hanako.SkyBox.GetAddressOf()));
    HR_T(CreateDDSTextureFromFile(m_pDevice, L"../Resource/Environment/Hanako/HanakoDiffuseHDR.dds", nullptr, Env_Hanako.Diffuse.GetAddressOf()));
    HR_T(CreateDDSTextureFromFile(m_pDevice, L"../Resource/Environment/Hanako/HanakoSpecularHDR.dds", nullptr, Env_Hanako.Specular.GetAddressOf()));
    HR_T(CreateDDSTextureFromFile(m_pDevice, L"../Resource/Environment/Hanako/HanakoBrdf.dds", nullptr, Env_Hanako.BRDF_LUT.GetAddressOf()));

    // Museum
    HR_T(CreateDDSTextureFromFile(m_pDevice, L"../Resource/Environment/Museum/MuseumEnvHDR.dds", nullptr, Env_Museum.SkyBox.GetAddressOf()));
    HR_T(CreateDDSTextureFromFile(m_pDevice, L"../Resource/Environment/Museum/MuseumDiffuseHDR.dds", nullptr, Env_Museum.Diffuse.GetAddressOf()));
    HR_T(CreateDDSTextureFromFile(m_pDevice, L"../Resource/Environment/Museum/MuseumSpecularHDR.dds", nullptr, Env_Museum.Specular.GetAddressOf()));
    HR_T(CreateDDSTextureFromFile(m_pDevice, L"../Resource/Environment/Museum/MuseumBrdf.dds", nullptr, Env_Museum.BRDF_LUT.GetAddressOf()));

    // Night
    HR_T(CreateDDSTextureFromFile(m_pDevice, L"../Resource/Environment/Night/NightEnvHDR.dds", nullptr, Env_Night.SkyBox.GetAddressOf()));
    HR_T(CreateDDSTextureFromFile(m_pDevice, L"../Resource/Environment/Night/NightDiffuseHDR.dds", nullptr, Env_Night.Diffuse.GetAddressOf()));
    HR_T(CreateDDSTextureFromFile(m_pDevice, L"../Resource/Environment/Night/NightSpecularHDR.dds", nullptr, Env_Night.Specular.GetAddressOf()));
    HR_T(CreateDDSTextureFromFile(m_pDevice, L"../Resource/Environment/Night/NightBrdf.dds", nullptr, Env_Night.BRDF_LUT.GetAddressOf()));

    // Street
    HR_T(CreateDDSTextureFromFile(m_pDevice, L"../Resource/Environment/Street/StreetEnvHDR.dds", nullptr, Env_Street.SkyBox.GetAddressOf()));
    HR_T(CreateDDSTextureFromFile(m_pDevice, L"../Resource/Environment/Street/StreetDiffuseHDR.dds", nullptr, Env_Street.Diffuse.GetAddressOf()));
    HR_T(CreateDDSTextureFromFile(m_pDevice, L"../Resource/Environment/Street/StreetSpecularHDR.dds", nullptr, Env_Street.Specular.GetAddressOf()));
    HR_T(CreateDDSTextureFromFile(m_pDevice, L"../Resource/Environment/Street/StreetBrdf.dds", nullptr, Env_Street.BRDF_LUT.GetAddressOf()));

    currentEnv = &Env_BakerSample;

    // HDR
    D3D11_TEXTURE2D_DESC hdrDesc = {};
    hdrDesc.Width = m_ClientWidth;
    hdrDesc.Height = m_ClientHeight;
    hdrDesc.ArraySize = 1;
    hdrDesc.MipLevels = 1;
    hdrDesc.SampleDesc.Count = 1;
    hdrDesc.Usage = D3D11_USAGE_DEFAULT;
    hdrDesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
    hdrDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
    hdrDesc.MiscFlags = 0;
    HR_T(m_pDevice->CreateTexture2D(&hdrDesc, nullptr, m_sceneHDRTex.GetAddressOf()));
    // RTV만들기
    HR_T(m_pDevice->CreateRenderTargetView(m_sceneHDRTex.Get(), nullptr, m_sceneHDRRTV.GetAddressOf()));
    // SRV 만들기
    HR_T(m_pDevice->CreateShaderResourceView(m_sceneHDRTex.Get(), nullptr, m_sceneHDRSRV.GetAddressOf()));

    // IA
    ID3DBlob* vsBlobToneMap = nullptr;
    CompileShaderFromFile(L"Shader/ToneMapping_VS.hlsl", "main", "vs_4_0", &vsBlobToneMap);

    HR_T(m_pDevice->CreateVertexShader(vsBlobToneMap->GetBufferPointer(),
        vsBlobToneMap->GetBufferSize(), NULL, m_toneMapVertexShader.GetAddressOf()));

    D3D11_INPUT_ELEMENT_DESC tonelayout[] =
    {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
    };

    HR_T(m_pDevice->CreateInputLayout(tonelayout, ARRAYSIZE(tonelayout), vsBlobToneMap->GetBufferPointer(),
        vsBlobToneMap->GetBufferSize(), m_toneMapInputLayout.GetAddressOf()));

    SAFE_RELEASE(vsBlobToneMap);

    ID3DBlob* psBlobToneMap = nullptr;
    CompileShaderFromFile(L"Shader/ToneMapping_PS.hlsl", "main", "ps_4_0", &psBlobToneMap);
    HR_T(m_pDevice->CreatePixelShader(psBlobToneMap->GetBufferPointer(),
        psBlobToneMap->GetBufferSize(), NULL, m_toneMapPixelShader.GetAddressOf()));

    SAFE_RELEASE(psBlobToneMap);

    D3D11_BUFFER_DESC bdToneMapCB = {};
    bdToneMapCB.Usage = D3D11_USAGE_DEFAULT;
    bdToneMapCB.ByteWidth = sizeof(ToneMapCB);
    bdToneMapCB.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    bdToneMapCB.CPUAccessFlags = 0;
    bdToneMapCB.MiscFlags = 0;

    HR_T(m_pDevice->CreateBuffer(&bdToneMapCB, nullptr, m_toneMapConstantBuffer.GetAddressOf()));

    // MainPass
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
    cbd.ByteWidth = sizeof(ConstantBuffer);
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

    m_ShadowPos = m_ShadowLookAt + (m_Light.Direction * m_ShadowUpDistFromLookAt);

    m_ShadowView = XMMatrixLookAtLH(m_ShadowPos, m_ShadowLookAt, Vector3(0.0f, 1.0f, 0.0f));

    m_pDeviceContext->RSSetViewports(1, &m_ShadowViewport);

    // Create Vertex Buffer
    ID3DBlob* vsBlob;
    CompileShaderFromFile(L"Shader/DepthOnly_VS.hlsl", "main", "vs_5_0", &vsBlob);

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
    m_pRenderTargetView.Reset();
    m_pDepthStencilView.Reset();

    SAFE_RELEASE(m_pSkyboxVertexBuffer);
    SAFE_RELEASE(m_pSkyboxIndexBuffer);
    SAFE_RELEASE(m_pSkyboxConstantBuffer);
    SAFE_RELEASE(m_pSkyboxDepthStencilState);

    SAFE_RELEASE(m_pShadowConstantBuffer);
    SAFE_RELEASE(m_pStaticMeshConstantBuffer);
}