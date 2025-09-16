#pragma once
#include <d3d11.h>
#include <dxgi1_2.h>

#include "../Common/GameApp.h"

class TutorialApp : public GameApp
{
public:
	TutorialApp(HINSTANCE hInstance);
	~TutorialApp();

	ID3D11Device* m_pDevice = nullptr;						// 디바이스
	ID3D11DeviceContext* m_pDeviceContext = nullptr;		// 즉시 디바이스 컨텍스트
	IDXGISwapChain* m_pSwapChain = nullptr;					// 스왑체인
	ID3D11RenderTargetView* m_pRenderTargetView = nullptr;	// 렌더링 타겟뷰

	ID3D11VertexShader* m_pVertexShader = nullptr;			// 정점 셰이더
	ID3D11PixelShader* m_pPixelShader = nullptr;			// 픽셀 셰이더
	ID3D11InputLayout* m_pInputLayout = nullptr;			// 입력 레이아웃

	ID3D11Buffer* m_pVertexBuffer = nullptr;				// 버텍스 버퍼
	UINT m_VertexBufferStride = 0;							// 버텍스 하나의 크기
	UINT m_VertexBufferOffset = 0;							// 버텍스 버퍼의 오프셋
	UINT m_VertexCount = 0;									// 버텍스 개수

	ID3D11Buffer* m_pIndexBuffer = nullptr;					// 인덱스 버퍼
	UINT m_IndexBufferOffset = 0;							// 인덱스 버퍼의 오프셋
	UINT m_IndexCount = 0;									// 인덱스 개수
	
	virtual bool Initialize(UINT Width, UINT Height);
	virtual void Update(float deltaTime);
	virtual void Render();

	bool InitD3D();
	void UninitD3D();

	bool InitScene();
	void UninitScene();
};

