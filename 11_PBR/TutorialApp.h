#pragma once
#include <windows.h>
#include "../Common/GameApp.h"
#include <d3d11.h>
#include <directxtk/SimpleMath.h>
#include "Mesh.h"
//#include "ImGuiManager.h"
//#include "ModelLoader.h"
//#include "SkeletalMesh.h"
//#include "SkeletalModel.h"

// refactoring
#include "StaticMesh.h"
#include "../Common/ConstantBuffer.h"
#include "../Common/Transform.h"

// ImGui
#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_dx11.h>
#include <Psapi.h>

using namespace DirectX::SimpleMath;
using namespace DirectX;

struct Object
{
    Object() = default;
    Object(const XMFLOAT3& pos) : transform(pos), initPos(pos) {}

    Vector3 initPos;

    Transform transform;

    Vector4 ambient = { 0.1f, 0.1f, 0.1f, 0.1f };
    Vector4 diffuse = { 1,1,1,1 };
    Vector4 specular = { 1,1,1,1 };

    float shininess = 200.f;

    Vector4 albedo = { 1.f, 1.f, 1.f, 1.0f };
    float roughness = 0.04f;
    float metallic = 0.0f;
    float gamma = 2.2f;

    void PosReset()
    {
        transform.SetPosition(initPos);
        transform.SetRotation(Vector3(0, 0, 0));
        transform.SetScale(Vector3(1, 1, 1));
    }

    void Reset()
    {
        // 기존 퐁 셰이딩
        //ambient = { 0.1f, 0.1f, 0.1f, 0.1f };
        //diffuse = { 1,1,1,1 };
        //specular = { 1,1,1,1 };
        //shininess = 200.f;

        // PBR
        albedo = { 1.f, 1.f, 1.f, 1.0f };
        roughness = 0.04f;
        metallic = 0.0f;
        gamma = 2.2f;
    }
};

struct Light
{
    Vector3 Direction = {0, -1, 1};
};

struct Environment
{
    ComPtr<ID3D11ShaderResourceView> SkyBox;
    ComPtr<ID3D11ShaderResourceView> Diffuse;
    ComPtr<ID3D11ShaderResourceView> Specular;
    ComPtr<ID3D11ShaderResourceView> BRDF_LUT;
};

class TutorialApp : public GameApp
{
public:
	TutorialApp(HINSTANCE hInstance);
	~TutorialApp();

	ID3D11Device* m_pDevice = nullptr;								// 디바이스	
	ID3D11DeviceContext* m_pDeviceContext = nullptr;				// 즉시 디바이스 컨텍스트
	IDXGISwapChain* m_pSwapChain = nullptr;							// 스왑체인

	ComPtr<ID3D11RenderTargetView> m_pRenderTargetView;				// 렌더링 타겟뷰
	ComPtr<ID3D11DepthStencilView> m_pDepthStencilView;				// 깊이값 처리를 위한 뎊스스텐실 뷰

	ComPtr<ID3D11VertexShader> m_pVertexShader;						// 정점 셰이더.
	ComPtr<ID3D11PixelShader> m_pPixelShader;						// 픽셀 셰이더.	
	ComPtr<ID3D11InputLayout> m_pInputLayout;						// 입력 레이아웃.
	ComPtr<ID3D11SamplerState> m_pSamplerLinear;					// 샘플러 스테이트
	ComPtr<ID3D11BlendState> m_pBlendState;							// 블렌더 스테이트

	ComPtr<ID3D11RasterizerState> m_pRasterStateNoCull;				// 컬링 여부(컬링 안함)
	ComPtr<ID3D11RasterizerState> m_pRasterStateBackCull;			// 컬링 여부(기본 값)
	ComPtr<ID3D11RasterizerState> m_pRasterStateFrontCull;			// 컬링 여부(앞면 컬링, 반전)

    ComPtr<ID3D11VertexShader> m_pSkeletalVS;                       // 본애니메이션 전용 VS
    ComPtr<ID3D11InputLayout> m_pSkeletalInputLayout;               // 본애니메이션 전용 IA

	/*------스카이박스-------*/
	ComPtr<ID3D11VertexShader> m_pSkyboxVertexShader;				// 정점 셰이더
	ComPtr<ID3D11PixelShader> m_pSkyboxPixelShader;					// 픽셀 셰이더
	ComPtr<ID3D11InputLayout> m_pSkyboxInputLayout;					// 입력 레이아웃
	ID3D11Buffer* m_pSkyboxVertexBuffer = nullptr;					// 버텍스 버퍼
	UINT m_SkyboxVertexBufferStride = 0;							// 버텍스 하나의 크기
	UINT m_SkyboxVertexBufferOffset = 0;							// 버텍스 버퍼의 오프셋
	ID3D11Buffer* m_pSkyboxIndexBuffer = nullptr;					// 인덱스 버퍼
	int m_nSkyboxIndices = 0;										// 인덱스 개수
	ID3D11Buffer* m_pSkyboxConstantBuffer = nullptr;				// 상수 버퍼
	ID3D11DepthStencilState* m_pSkyboxDepthStencilState = nullptr;	// 뎊스 스텐실 스테이트
    
	ID3D11ShaderResourceView* m_pCubeTextureRV = nullptr;			// 텍스처 파일(큐브맵, 현재 큐브맵)

	ID3D11ShaderResourceView* m_pCubeMuseumTextureRV = nullptr;		// 텍스처 파일1(큐브맵, 실내)
	ID3D11ShaderResourceView* m_pCubeDaylightTextureRV = nullptr;	// 텍스처 파일2(큐브맵, 실외)
	ID3D11ShaderResourceView* m_pCubeHanakoTextureRV = nullptr;		// 텍스처 파일3(큐브맵, 디버그)

	Matrix m_World;
	Matrix m_View;
	Matrix m_Projection;

    D3D11_VIEWPORT m_MainViewport;

    /*-------PBR-------*/
    float ambientOcclusion = 1.0f;

    /*-------IBL------*/
    ComPtr<ID3D11ShaderResourceView> m_pIBLCubeSRV;                 // IBL 텍스처 파일(큐브맵, 현재 Env)
    ComPtr<ID3D11ShaderResourceView> m_pIBLDiffuseSRV;              // IBL Diffuse 파일
    ComPtr<ID3D11ShaderResourceView> m_pIBLSpecularSRV;             // IBL Specular 파일
    ComPtr<ID3D11ShaderResourceView> m_pIBLBDRFLutSRV;              // IBL BDRF 파일
    
    Environment* currentEnv;

    Environment Env_Museum;
    Environment Env_DayLight;
    Environment Env_Night;
    Environment Env_Street;
    Environment Env_BakerSample;
    Environment Env_Hanako; // Hanako

    /*-----Shadow Map-----*/
    ComPtr<ID3D11Texture2D> m_pShadowMap;
    ComPtr<ID3D11DepthStencilView> m_pShadowMapDSV;
    ComPtr<ID3D11ShaderResourceView> m_pShadowMapSRV;

    Vector3 m_ShadowPos;
    Vector3 m_ShadowLookAt;
    Matrix m_ShadowView;
    Matrix m_ShadowProjection;

    ComPtr<ID3D11RasterizerState>    m_pShadowRasterState;  // 레스터라이저

    ID3D11Buffer* m_pShadowConstantBuffer = nullptr;

    const UINT SHADOW_WIDTH = 8192;     // 그림자 해상도
    const UINT SHADOW_HEIGHT = 8192;

    float m_ShadowForwardDistFromCamera = 500.0;        // 카메라에서 앞쪽으로 얼마나 떨어진 위치에 그림자 카메라를 둘지
    float m_ShadowUpDistFromLookAt = 1500.0f;              // LookAt 지점에서 얼마나 위쪽으로 올릴지
	
    Vector2 m_ShadowProjectionNearFar = { 690, 5000 };

    D3D11_VIEWPORT m_ShadowViewport;

	ComPtr<ID3D11VertexShader> m_pShadowVS;				// 정점 셰이더


    // Imgui에 전달할 함수들
    //ID3D11ShaderResourceView* GetShadowSRV() const { return m_pShadowMapSRV.Get(); }
    //UINT GetShadowMapWidth() const { return SHADOW_WIDTH; }
    //UINT GetShadowMapHeight() const { return SHADOW_HEIGHT; }

    // Manager
	//ImGuiManager m_ImGuiManager;
	//ModelLoader m_ModelLoader;
	//SkeletalMesh m_SkeletalModelLoader;

	TimeSystem m_TimeSystem;


    ID3D11Buffer* m_pStaticMeshConstantBuffer;						// 상수 버퍼.
    StaticMesh m_StaticMesh;
    StaticMesh ground;


	// ModelLoad
	//Model* GetModelByName(const std::string& name)
	//{
	//	for (auto& model : m_ModelLoader.models_)
	//	{
	//		if (model.name == name) return &model;
	//	}

	//	for (auto& model : m_SkeletalModelLoader.models_)
	//	{
	//		if (model.name == name) return &model;
	//	}

	//	return nullptr;
	//}

 //   SkeletalModel* GetSkeletalModelByName(const std::string& name)
 //   {
 //       for (auto& model : m_SkeletalModelLoader.models_)
 //       {
 //           if (model.name == name) return &model;
 //       }

 //       return nullptr;
 //   }

	//float m_Angle = 0.0f;

	// 라이트 관련
	// 씬 안에서 라이트는 여러개 존재할 수 있으며, 이를 묶어서 처리할 수도 있음
	XMFLOAT4 m_AmbientColor = { 0.1f, 0.1f, 0.1f, 1.0f };
	XMFLOAT4 m_DiffuseColor = { 1.0f, 1.0f, 1.0f, 1.0f };
	XMFLOAT4 m_SpecularColor = { 1.0f, 1.0f, 1.0f, 1.0f };
	XMFLOAT4 m_InitialLightDirs = { 1.0f, -1.0f, 1.0f, 0.0f };
	XMFLOAT4 m_LightDirsEvaluated = {};
    Vector4 m_shininess = { 200, 0, 0, 0 };

    Light m_Light;

	Vector4 m_cameraPos;

    // ImGui
    Object object1 = { Vector3(0,0,0) };
    Object object2 = { Vector3(-100,0,0) };
    Object object3 = { Vector3(100,0,0) };

    float FOV = 1.0f;

    Vector4 ambientLight = {  };
    Vector4 diffuseLight = {  };
    Vector4 specularLight = {  };
    Vector3 lightDir = {  };

    //bool viewChanger = false;

    Vector2 depth = { 1.0f, 10000.0f };

    // PBR
    //Vector4 m_Albedo = { 1.f, 1.f, 1.f, 1.0f };
    //float m_Roughness = 0.5f;
    //float m_Metalness = 0.0f;

    bool useTexture = true;
    bool useCustomAlbedo = false;
    bool useLighting = true;
    bool useIBL = true;

    void LightReset()
    {
        ambientLight = { 0.1f, 0.1f, 0.1f, 0.1f };
        diffuseLight = { 1.0f, 1.0f, 1.0f, 1.0f };
        specularLight = { 1.0f, 1.0f, 1.0f, 1.0f };
        lightDir = { 0.0f, -1.0f, 1.0f };
    }

    void RenderObjectUI(const char* label, Object& obj)
    {
        ImGui::SliderFloat3((std::string(label) + u8" 크기").c_str(), &obj.transform.scale.x, 1.0f, 100.0f);
        ImGui::SliderFloat3((std::string(label) + u8" 위치").c_str(), &obj.transform.position.x, -100.0f, 100.0f, "%.1f");
        ImGui::DragFloat3((std::string(label) + u8" 회전").c_str(), &obj.transform.rotation.x, 0.1f);
        ImGui::ColorEdit4((std::string(label) + u8" Material Ambient").c_str(), &obj.ambient.x);
        ImGui::ColorEdit4((std::string(label) + u8" Material Diffuse").c_str(), &obj.diffuse.x);
        ImGui::ColorEdit4((std::string(label) + u8" Material Specular").c_str(), &obj.specular.x);
        ImGui::SliderFloat((std::string(label) + u8" 광택지수").c_str(), &obj.shininess, 200.0f, 20000.0f);
        if (ImGui::Button((std::string(label) + u8" 초기화").c_str())) obj.Reset();
        ImGui::Text("");
    }

	bool Initialize(UINT Width, UINT Height) override;
	void Update() override;
	void Render() override;

	bool InitD3D();
	void UninitD3D();

	bool InitImGUI();
	void UninitImGUI();

	bool InitScene();
	void UninitScene();
};