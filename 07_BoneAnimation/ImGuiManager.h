#pragma once
#include "../Common/GameApp.h"
#include "../Common/Camera.h"
#include "../Common/Transform.h"

#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_dx11.h>
#include <Psapi.h>

#include <directxtk/SimpleMath.h>
#include <vector>

using namespace DirectX::SimpleMath;

struct Object
{
	Object() = default;
	Object(const XMFLOAT3& pos) : transform(pos), initPos(pos) {}

	Vector3 initPos;

	Transform transform;

	Vector4 ambient = {0.1f, 0.1f, 0.1f, 0.1f};
	Vector4 diffuse = {1,1,1,1};
	Vector4 specular = {1,1,1,1};

	float shininess = 1000.f;
	
	void Reset()
	{
		transform.SetPosition(initPos);
		transform.SetRotation(Vector3(0, 0, 0));
		transform.SetScale(Vector3(1, 1, 1));

		ambient = { 0.1f, 0.1f, 0.1f, 0.1f };
		diffuse = { 1,1,1,1 };
		specular = { 1,1,1,1 };

		shininess = 1000.f;
	}
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

	Object object1 = { Vector3(0,0,0)};
	Object object2 = { Vector3(0,0,0) };
	Object object3 = { Vector3(100,0,0) };

	float FOV = 1.0f;

	Vector4 ambientLight = {  };
	Vector4 diffuseLight = {  };
	Vector4 specularLight = {  };
	Vector3 lightDir = {  };

	bool viewChanger = false;

	Vector2 depth = { 1.0f, 10000.0f };

	bool useLighting = true;

private:
	void EndFrame();

	float nearZ = 1.0f;
	float farZ = 1000.0f;

	GameApp* m_app;

	void LightReset()
	{
		ambientLight = { 0.1f, 0.1f, 0.1f, 0.1f };
		diffuseLight = { 1.0f, 1.0f, 1.0f, 1.0f };
		specularLight = { 1.0f, 1.0f, 1.0f, 1.0f };
		lightDir = { 0.0f, 0.0f, 1.0f };
	}

	void RenderObjectUI(const char* label, Object& obj);
};