#pragma once
#include "../Common/GameApp.h"
#include "../Common/Camera.h"

#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_dx11.h>
#include <Psapi.h>

#include <directxtk/SimpleMath.h>

using namespace DirectX::SimpleMath;

class ImGuiManager
{
public:
	ImGuiManager() = default;
	//ImGuiManager(GameApp* app) : m_app(app) {}

	void Initialize();
	void BeginFrame(ID3D11Device* _Device, ID3D11DeviceContext* _DeviceContext);
	void Release() { EndFrame(); }
	void Render();

	float GetObjectScale() { return objectScaleXYZ; }
	Vector3 GetScale() { return objectScale; }

	float FOV = 1.0f;

	Vector3 objectTransform = {  };
	Vector2 objectRotate = {  };
	Vector4 objectAmbient = {  };
	Vector4 objectDiffuse = {  };
	Vector4 objectSpecular = {  };

	float shininess = 1000.0f;

	Vector4 ambientLight = {  };
	Vector4 diffuseLight = {  };
	Vector4 specularLight = {  };
	Vector3 lightDir = {  };

	bool viewChanger = false;

	Vector2 depth = { 1.0f, 10000.0f };

	bool useLighting = true;

private:
	void EndFrame();

	Camera m_Camera;

	float nearZ = 1.0f;
	float farZ = 1000.0f;

	float objectScaleXYZ = 10.0f;
	Vector3 objectScale = {10,10,10};

	GameApp* m_app;

	void ObjectReset()
	{
		objectScaleXYZ = 10.0f;
		objectTransform = { 0.0f , 0.0f, 0.0f };
		objectRotate = { 0.0f, 0.0f };

		objectAmbient = { 0.1f, 0.1f, 0.1f, 0.1f };
		objectDiffuse = { 1.0f, 1.0f, 1.0f, 1.0f };
		objectSpecular = { 1.0f, 1.0f, 1.0f, 1.0f };
	}

	void LightReset()
	{
		ambientLight = { 0.1f, 0.1f, 0.1f, 0.1f };
		diffuseLight = { 1.0f, 1.0f, 1.0f, 1.0f };
		specularLight = { 1.0f, 1.0f, 1.0f, 1.0f };
		lightDir = { 0.0f, 0.0f, 1.0f };
		shininess = 1000.0f;
	}
};