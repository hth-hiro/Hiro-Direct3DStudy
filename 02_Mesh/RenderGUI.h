#pragma once
#include "../Common/GameApp.h"

#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_dx11.h>
#include <Psapi.h>

#include <directxtk/SimpleMath.h>
using namespace DirectX::SimpleMath;

class RenderGUI : public GameApp
{
public:
	RenderGUI() : GameApp(GetModuleHandleW(nullptr)) {}
	RenderGUI(HINSTANCE hInstace) : GameApp(hInstace) {}

	void Initialize(ID3D11Device* _Device, ID3D11DeviceContext* _DeviceContext);
	void Render() override;
	void Release();

	float playerPosX = 0.0f;
	float playerPosY = 0.0f;
	float playerPosZ = -10.0f;
	
	float m_ParentWorldX = 0.0f;
	float m_ParentWorldY = 0.0f;
	float m_ParentWorldZ = 0.0f;

	float m_Child1LocalX = 8.0f;
	float m_Child1LocalY = 0.0f;
	float m_Child1LocalZ = 0.0f;

	float m_Child2LocalX = 3.0f;
	float m_Child2LocalY = 0.0f;
	float m_Child2LocalZ = 0.0f;

	float FOV = 1.0f;

	float nearZ = 0.01f;
	float farZ = 100.0f;

	bool isFocusParent = false;

	int m_counter = 0;
};

