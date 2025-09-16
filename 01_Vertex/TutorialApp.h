#pragma once
#include <d3d11.h>
#include <dxgi1_2.h>

#include "../Common/GameApp.h"

class TutorialApp : public GameApp
{
public:
	TutorialApp(HINSTANCE hInstance);
	~TutorialApp();

	ID3D11Device* m_pDevice = nullptr;						// ����̽�
	ID3D11DeviceContext* m_pDeviceContext = nullptr;		// ��� ����̽� ���ؽ�Ʈ
	IDXGISwapChain* m_pSwapChain = nullptr;					// ����ü��
	ID3D11RenderTargetView* m_pRenderTargetView = nullptr;	// ������ Ÿ�ٺ�

	ID3D11VertexShader* m_pVertexShader = nullptr;			// ���� ���̴�
	ID3D11PixelShader* m_pPixelShader = nullptr;			// �ȼ� ���̴�
	ID3D11InputLayout* m_pInputLayout = nullptr;			// �Է� ���̾ƿ�

	ID3D11Buffer* m_pVertexBuffer = nullptr;				// ���ؽ� ����
	UINT m_VertexBufferStride = 0;							// ���ؽ� �ϳ��� ũ��
	UINT m_VertexBufferOffset = 0;							// ���ؽ� ������ ������
	UINT m_VertexCount = 0;									// ���ؽ� ����

	ID3D11Buffer* m_pIndexBuffer = nullptr;					// �ε��� ����
	UINT m_IndexBufferOffset = 0;							// �ε��� ������ ������
	UINT m_IndexCount = 0;									// �ε��� ����
	
	virtual bool Initialize(UINT Width, UINT Height);
	virtual void Update(float deltaTime);
	virtual void Render();

	bool InitD3D();
	void UninitD3D();

	bool InitScene();
	void UninitScene();
};

