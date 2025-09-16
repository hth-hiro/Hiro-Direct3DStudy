#pragma once
#include <windows.h>
#include "../Common/GameApp.h"
#include <d3d11.h>
#include <directxtk/SimpleMath.h>

#include "RenderGUI.h"

using namespace DirectX::SimpleMath;
using namespace DirectX;

class TutorialApp : public GameApp
{
public:
	TutorialApp(HINSTANCE hInstance);
	~TutorialApp();

	ID3D11Device* m_pDevice = nullptr;						// 디바이스	
	ID3D11DeviceContext* m_pDeviceContext = nullptr;		// 즉시 디바이스 컨텍스트
	IDXGISwapChain* m_pSwapChain = nullptr;					// 스왑체인
	ID3D11RenderTargetView* m_pRenderTargetView = nullptr;	// 렌더링 타겟뷰
	ID3D11DepthStencilView* m_pDepthStencilView = nullptr;  // 깊이값 처리를 위한 뎊스스텐실 뷰

	// 렌더링 파이프라인에 적용하는  객체와 정보
	ID3D11VertexShader* m_pVertexShader = nullptr;		// 정점 셰이더.
	ID3D11PixelShader* m_pPixelShader = nullptr;		// 픽셀 셰이더.	
	//ID3D11PixelShader* m_pPixelShaderSolid = nullptr;	// 픽셀 셰이더 라이트 표시용.	

	ID3D11InputLayout* m_pInputLayout = nullptr;		// 입력 레이아웃.
	ID3D11Buffer* m_pVertexBuffer = nullptr;			// 버텍스 버퍼.
	UINT m_VertexBufferStride = 0;						// 버텍스 하나의 크기.
	UINT m_VertexBufferOffset = 0;						// 버텍스 버퍼의 오프셋.
	ID3D11Buffer* m_pIndexBuffer = nullptr;				// 버텍스 버퍼.
	int m_nIndices = 0;									// 인덱스 개수.
	ID3D11Buffer* m_pConstantBuffer = nullptr;			// 상수 버퍼.

	ID3D11SamplerState* m_pSamplerLinear = nullptr;			// 샘플러 스테이트
	ID3D11ShaderResourceView* m_pTextureRV = nullptr;		// 텍스처 파일

	// 스카이박스용
	ID3D11VertexShader* m_pSkyboxVertexShader = nullptr;// 스카이 박스용 정점 셰이더.
	ID3D11PixelShader* m_pSkyboxPixelShader = nullptr;	// 스카이박스용 픽셀 셰이더

	ID3D11InputLayout* m_pSkyboxInputLayout = nullptr;	// 스카이 박스 입력 레이아웃.
	ID3D11Buffer* m_pSkyboxVertexBuffer = nullptr;		// 버텍스 버퍼.
	UINT m_SkyboxVertexBufferStride = 0;				// 버텍스 하나의 크기.
	UINT m_SkyboxVertexBufferOffset = 0;				// 버텍스 버퍼의 오프셋.
	ID3D11Buffer* m_pSkyboxIndexBuffer = nullptr;		// 인덱스 버퍼.
	int m_nSkyboxIndices = 0;							// 인덱스 개수.
	ID3D11Buffer* m_pSkyboxConstantBuffer = nullptr;			// 상수 버퍼.
	ID3D11ShaderResourceView* m_pCubeTextureRV = nullptr;	// 텍스처 파일(큐브맵, 현재 큐브맵)

	ID3D11ShaderResourceView* m_pCubeMuseumTextureRV = nullptr;	// 텍스처 파일(큐브맵)
	ID3D11ShaderResourceView* m_pCubeDaylightTextureRV = nullptr;	// 텍스처 파일(큐브맵)

	ID3D11DepthStencilState* m_pSkyboxDepthStencilState = nullptr;


	Matrix m_World;
	Matrix m_View;
	Matrix m_Projection;

	RenderGUI m_GUI;

	//float m_Angle = 0.0f;

	// 라이트 관련
	// 씬 안에서 라이트는 여러개 존재할 수 있다. 이를 묶어서 처리할 수도 있음
	//XMFLOAT4 m_LightColors[2] =
	//{
	//	XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
	//	XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f)
	//};
	//XMFLOAT4 m_InitialLightDirs[2] =
	//{
	//	XMFLOAT4(-0.577f, 0.577f, -0.577f, 1.0f),
	//	XMFLOAT4(0.0f, 0.0f, -1.0f, 1.0f),
	//};
	//XMFLOAT4 m_LightDirsEvaluated[2] = {};

	// 지금 쓰고 있는건 1개
	XMFLOAT4 m_LightColor = { 1.0f, 1.0f, 1.0f, 1.0f };
	XMFLOAT4 m_InitialLightDirs = { -0.577f, 0.577f, -0.577f, 0.0f };
	XMFLOAT4 m_LightDirsEvaluated = {};

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

