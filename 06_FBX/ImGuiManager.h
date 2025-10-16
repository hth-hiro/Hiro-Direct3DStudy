#pragma once
#include "../Common/GameApp.h"
#include "../Common/Camera.h"
#include "../Common/Transform.h"

#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_dx11.h>
#include <Psapi.h>

#include <directxtk/SimpleMath.h>

using namespace DirectX::SimpleMath;

struct Object
{
	Transform transform;

	Vector4 ambient;
	Vector4 diffuse;
	Vector4 specular;

	float shininess;
};

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

	Object object1 = {};
	Object object2 = {};
	Object object3 = {};

	float FOV = 1.0f;

	Vector3 objectTransform = {  };
	Vector2 objectRotate = {  };
	Vector4 objectAmbient = {  };
	Vector4 objectDiffuse = {  };
	Vector4 objectSpecular = {  };

	float shininess = 1000.0f;

	Vector4 ambientColor = {  };
	Vector4 diffuseColor = {  };
	Vector4 specularColor = {  };
	Vector3 lightDir = {  };

	bool viewChanger = false;

	Vector2 depth = { 1.0f, 10000.0f };

	bool useLighting = true;

private:
	void EndFrame();

	Camera m_Camera;

	float nearZ = 1.0f;
	float farZ = 1000.0f;

	float objectScaleXYZ = 1.0f;

	GameApp* m_app;

	void ObjectReset()
	{
		objectScaleXYZ = 1.0f;
		objectTransform = { 0.0f , 0.0f, 0.0f };
		objectRotate = { 0.0f, 0.0f };

		objectAmbient = { 0.1f, 0.1f, 0.1f, 0.1f };
		objectDiffuse = { 1.0f, 1.0f, 1.0f, 1.0f };
		objectSpecular = { 1.0f, 1.0f, 1.0f, 1.0f };

		shininess = 1000.0f;
	}

	void LightReset()
	{
		ambientColor = { 0.1f, 0.1f, 0.1f, 0.1f };
		diffuseColor = { 1.0f, 1.0f, 1.0f, 1.0f };
		specularColor = { 1.0f, 1.0f, 1.0f, 1.0f };
		lightDir = { 0.0f, 0.0f, 1.0f };
	}

	void RenderObjectUI(const char* label, Object& obj);
};