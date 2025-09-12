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

	float m_PlayerPosX = 0.0f;
	float m_PlayerPosY = 0.0f;
	float m_PlayerPosZ = -10.0f;

	float m_ObjectPosX = 0.0f;
	float m_ObjectPosY = 0.0f;
	float m_ObjectPosZ = 0.0f;

	float m_LightPosX = 0.0f;
	float m_LightPosY = 0.0f;
	float m_LightPosZ = 0.0f;

	float m_FOV = 1.0f;

	float m_NearZ = 0.01f;
	float m_FarZ = 100.0f;

	bool m_FocusParent = false;

	int m_counter = 0;
};

