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

	ID3D11Device* m_pDevice = nullptr;						// ����̽�	
	ID3D11DeviceContext* m_pDeviceContext = nullptr;		// ��� ����̽� ���ؽ�Ʈ
	IDXGISwapChain* m_pSwapChain = nullptr;					// ����ü��
	ID3D11RenderTargetView* m_pRenderTargetView = nullptr;	// ������ Ÿ�ٺ�
	ID3D11DepthStencilView* m_pDepthStencilView = nullptr;  // ���̰� ó���� ���� �X�����ٽ� ��

	// ������ ���������ο� �����ϴ�  ��ü�� ����
	ID3D11VertexShader* m_pVertexShader = nullptr;		// ���� ���̴�.
	ID3D11PixelShader* m_pPixelShader = nullptr;		// �ȼ� ���̴�.	
	//ID3D11PixelShader* m_pPixelShaderSolid = nullptr;	// �ȼ� ���̴� ����Ʈ ǥ�ÿ�.	

	ID3D11InputLayout* m_pInputLayout = nullptr;		// �Է� ���̾ƿ�.
	ID3D11Buffer* m_pVertexBuffer = nullptr;			// ���ؽ� ����.
	UINT m_VertexBufferStride = 0;						// ���ؽ� �ϳ��� ũ��.
	UINT m_VertexBufferOffset = 0;						// ���ؽ� ������ ������.
	ID3D11Buffer* m_pIndexBuffer = nullptr;				// ���ؽ� ����.
	int m_nIndices = 0;									// �ε��� ����.
	ID3D11Buffer* m_pConstantBuffer = nullptr;			// ��� ����.

	ID3D11SamplerState* m_pSamplerLinear = nullptr;			// ���÷� ������Ʈ
	ID3D11ShaderResourceView* m_pTextureRV = nullptr;		// �ؽ�ó ����

	// ��ī�̹ڽ���
	ID3D11VertexShader* m_pSkyboxVertexShader = nullptr;// ��ī�� �ڽ��� ���� ���̴�.
	ID3D11PixelShader* m_pSkyboxPixelShader = nullptr;	// ��ī�̹ڽ��� �ȼ� ���̴�

	ID3D11InputLayout* m_pSkyboxInputLayout = nullptr;	// ��ī�� �ڽ� �Է� ���̾ƿ�.
	ID3D11Buffer* m_pSkyboxVertexBuffer = nullptr;		// ���ؽ� ����.
	UINT m_SkyboxVertexBufferStride = 0;				// ���ؽ� �ϳ��� ũ��.
	UINT m_SkyboxVertexBufferOffset = 0;				// ���ؽ� ������ ������.
	ID3D11Buffer* m_pSkyboxIndexBuffer = nullptr;		// �ε��� ����.
	int m_nSkyboxIndices = 0;							// �ε��� ����.
	ID3D11Buffer* m_pSkyboxConstantBuffer = nullptr;			// ��� ����.
	ID3D11ShaderResourceView* m_pCubeTextureRV = nullptr;	// �ؽ�ó ����(ť���, ���� ť���)

	ID3D11ShaderResourceView* m_pCubeMuseumTextureRV = nullptr;	// �ؽ�ó ����(ť���)
	ID3D11ShaderResourceView* m_pCubeDaylightTextureRV = nullptr;	// �ؽ�ó ����(ť���)

	ID3D11DepthStencilState* m_pSkyboxDepthStencilState = nullptr;


	Matrix m_World;
	Matrix m_View;
	Matrix m_Projection;

	RenderGUI m_GUI;

	//float m_Angle = 0.0f;

	// ����Ʈ ����
	// �� �ȿ��� ����Ʈ�� ������ ������ �� �ִ�. �̸� ��� ó���� ���� ����
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

	// ���� ���� �ִ°� 1��
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

