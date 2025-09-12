#pragma once
#include "../Common/GameApp.h"

#include "RenderGUI.h"

#include <crtdbg.h>
#include <vector>

#include <dxtmpl.h>
#include <directxtk/SimpleMath.h>
#include <d3d11.h>
#include <dxgi1_4.h>

#include <imgui.h>	// gui 추가
#include <imgui_impl_win32.h>
#include <imgui_impl_dx11.h>

#include <Psapi.h>	// Process Status API
// (Windows에서 현재 실행 중인 프로세스, 모듈, 메모리 사용량 등을 조회할 수 있는 함수 제공

#include <string>
#include <wrl/client.h>

#pragma comment (lib, "d3d11.lib")
#pragma comment(lib,"dxgi.lib")

using namespace DirectX::SimpleMath;
using namespace DirectX;
using namespace Microsoft::WRL;
using namespace std;

struct ConstantBuffer
{
	Matrix mWorld;
	Matrix mView;
	Matrix mProjection;
};

struct Vertex
{
	Vector3 position;
	Vector4 color;

	Vertex(float x, float y, float z) : position(x, y, z) {}
	Vertex(Vector3 position) : position(position) {}
	Vertex(Vector3 position, Vector4 color)
		: position(position), color(color) {
	}
};

struct Object
{
	Matrix local = Matrix::Identity;
	Matrix world = Matrix::Identity;
	Object* parent = nullptr;
	vector<Object*> children;

	vector<Vertex> m_Vertices;
	vector<WORD> m_Indices;

	ID3D11Buffer* m_pVertexBuffer = nullptr;
	ID3D11Buffer* m_pIndexBuffer = nullptr;

	void AddChild(Object* obj)
	{
		obj->parent = this;
		children.push_back(obj);
	}
};

class TutorialApp : public GameApp
{
public:
	TutorialApp(HINSTANCE hInstance);
	~TutorialApp();

	ComPtr<IDXGIFactory4> m_pDxGIFactory;	// DXGI팩토리
	ComPtr<IDXGIAdapter3> m_pDXGIAdapter;	// 비디오 카드에 접근 가능한 인터페이스

	// 렌더링 파이프라인을 구성하는 필수 인터페이스
	ID3D11Device* m_pDevice = nullptr;
	ID3D11DeviceContext* m_pDeviceContext = nullptr;
	IDXGISwapChain* m_pSwapChain = nullptr;
	ID3D11RenderTargetView* m_pRenderTargetView = nullptr;

	ID3D11VertexShader* m_pVertexShader = nullptr;
	ID3D11PixelShader* m_pPixelShader = nullptr;
	ID3D11InputLayout* m_pInputLayout = nullptr;

	//ID3D11Buffer* m_pVertexBuffer = nullptr;
	UINT m_VertexBufferStride = 0;
	UINT m_VertexBufferOffset = 0;
	//UINT m_VertexCount = 0;	// 사실상 인덱스카운트가 있어서 필요가 없음

	//ID3D11Buffer* m_pIndexBuffer = nullptr;
	UINT m_IndexBufferOffset = 0;
	//UINT m_IndexCount = 0;

	ID3D11Buffer* m_pConstantBuffer = nullptr;

	ID3D11DepthStencilView* m_pDepthStencilView = nullptr;

	Matrix	m_World;
	Matrix	m_View;
	Matrix	m_Projection;	// NDC 변환 행렬

	float m_ParentAngle = 0.0f;
	float m_Child1Angle = 0.0f;
	float m_Child2Angle = 0.0f;

	RenderGUI m_GUI;

	virtual bool Initialize(UINT Width, UINT Height);
	virtual void Update();
	virtual void Render();

	bool InitD3D();
	void UninitD3D();

	bool InitImGUI();
	void UninitImGUI();

	bool InitScene();
	void UninitScene();

	Object m_ParentObj;
	Object m_ChildObj1;
	Object m_ChildObj2;

	void UpdateWorld(Object& obj)
	{
		if (obj.parent)
		{
			// 부모 오브젝트가 있다면 자식의 월드 좌표는 로컬좌표 * 부모의 월드좌표
			obj.world = obj.local * obj.parent->world;
		}
		else
		{
			// 부모 오브젝트가 없다면 월드좌표가 곧 로컬좌표임
			obj.world = obj.local;
		}

		for (auto child : obj.children)
		{
			UpdateWorld(*child);
		}
	}

	void RenderObject(ID3D11DeviceContext* context, Object& obj)
	{
		ConstantBuffer cb;
		cb.mWorld = XMMatrixTranspose(obj.world);
		cb.mView = XMMatrixTranspose(m_View);
		cb.mProjection = XMMatrixTranspose(m_Projection);

		context->UpdateSubresource(m_pConstantBuffer, 0, nullptr, &cb, 0, 0);

		context->IASetVertexBuffers(0, 1, &obj.m_pVertexBuffer, &m_VertexBufferStride, &m_VertexBufferOffset);
		context->IASetIndexBuffer(obj.m_pIndexBuffer, DXGI_FORMAT_R16_UINT, m_IndexBufferOffset);

		context->DrawIndexed(obj.m_Indices.size(), 0, 0);

		for (auto child : obj.children)
			RenderObject(context, *child);
	}
};

