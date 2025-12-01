#pragma once
#include <windows.h>
#include "../Common/GameApp.h"
#include <d3d11.h>
#include <directxtk/SimpleMath.h>
#include "Mesh.h"
#include "ImGuiManager.h"
#include "ModelLoader.h"
#include "SkeletalMesh.h"
#include "SkeletalModel.h"

// refactoring
#include "StaticMesh.h"
#include "../Common/ConstantBuffer.h"

using namespace DirectX::SimpleMath;
using namespace DirectX;

struct Light
{
    Vector3 Direction = {0, -1, 1};
};

enum ERasterizeState
{
	None, Default, Back = Default, Front
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

    float m_ShadowForwardDistFromCamera = 10.0f;        // 카메라에서 앞쪽으로 얼마나 떨어진 위치에 그림자 카메라를 둘지
    float m_ShadowUpDistFromLookAt = 2.0f;              // LookAt 지점에서 얼마나 위쪽으로 올릴지
	
    Vector2 m_ShadowProjectionNearFar = { 0.01f, 100.0f };

    D3D11_VIEWPORT m_ShadowViewport;

	ComPtr<ID3D11VertexShader> m_pShadowVS;				// 정점 셰이더


    // Imgui에 전달할 함수들
    ID3D11ShaderResourceView* GetShadowSRV() const { return m_pShadowMapSRV.Get(); }
    UINT GetShadowMapWidth() const { return SHADOW_WIDTH; }
    UINT GetShadowMapHeight() const { return SHADOW_HEIGHT; }

    // Manager
	ImGuiManager m_ImGuiManager;
	ModelLoader m_ModelLoader;
	SkeletalMesh m_SkeletalModelLoader;

	TimeSystem m_TimeSystem;


    ID3D11Buffer* m_pStaticMeshConstantBuffer;						// 상수 버퍼.
    StaticMesh m_StaticMesh;
    StaticMesh ground;


	// ModelLoad
	Model* GetModelByName(const std::string& name)
	{
		for (auto& model : m_ModelLoader.models_)
		{
			if (model.name == name) return &model;
		}

		for (auto& model : m_SkeletalModelLoader.models_)
		{
			if (model.name == name) return &model;
		}

		return nullptr;
	}

    SkeletalModel* GetSkeletalModelByName(const std::string& name)
    {
        for (auto& model : m_SkeletalModelLoader.models_)
        {
            if (model.name == name) return &model;
        }

        return nullptr;
    }

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

	bool Initialize(UINT Width, UINT Height) override;
	void Update() override;
	void Render() override;

	bool InitD3D();
	void UninitD3D();

	bool InitImGUI();
	void UninitImGUI();

	bool InitScene();
	void UninitScene();

private:
	void RSSetState(ERasterizeState state)
	{

	}
};