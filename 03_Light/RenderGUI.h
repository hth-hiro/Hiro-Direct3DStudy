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
	//RenderGUI() : GameApp(GetModuleHandleW(nullptr)) {}
	RenderGUI(HINSTANCE hInstace) : GameApp(hInstace) {}

	void Initialize(ID3D11Device* _Device, ID3D11DeviceContext* _DeviceContext);
	void Render() override;
	void Release();

	// 카메라의 위치
	float playerPosX = 0.0f;
	float playerPosY = 0.0f;
	float playerPosZ = -10.0f;

	// 오브젝트
	// 위치
	float objectPosX = 0.0f;
	float objectPosY = 0.0f;
	float objectPosZ = 0.0f;

	// 회전
	float objectYaw = 45.0f;
	float objectPitch = 0.0f;
	float objectRoll = 0.0f;

	// 빛
	// 위치
	float lightPosX = 0.0f;
	float lightPosY = 0.0f;
	float lightPosZ = 0.0f;

	// 색상
	float lightColorR = 1.0f;
	float lightColorG = 1.0f;
	float lightColorB = 1.0f;

	// 방향
	float lightDirX = -0.577f;
	float lightDirY = 0.577f;
	float lightDirZ = -0.577f;

	float FOV = 1.0f;

	float nearZ = 0.01f;
	float farZ = 100.0f;

	bool isFocusParent = false;
};

