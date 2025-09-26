#pragma once
#include "../Common/GameApp.h"
#include "../Common/Camera.h"

#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_dx11.h>
#include <Psapi.h>

#include <directxtk/SimpleMath.h>

using namespace DirectX::SimpleMath;

class IMGUI
{
public:
	IMGUI(GameApp* app) : m_app(app) {}

	void Initialize(ID3D11Device* _Device, ID3D11DeviceContext* _DeviceContext);
	void Render();
	void Release();

	Camera m_Camera;

	float FOV = 1.0f;

	float nearZ = 0.01f;
	float farZ = 50000.0f;

	// 오브젝트
	// 크기
	float objectScaleXYZ = 200.0f;

	// 회전
	float objectYaw = 0.0f;
	float objectPitch = 0.0f;
	float objectRoll = 0.0f;

	// 위치
	float objectPosX = 0.0f;
	float objectPosY = 0.0f;
	float objectPosZ = 0.0f;

	// 오브젝트 머터리얼
	float objectAmbientR = 0.1f;
	float objectAmbientG = 0.1f;
	float objectAmbientB = 0.1f;
	float objectAmbientA = 0.1f;

	float objectDiffuseR = 1.0f;
	float objectDiffuseG = 1.0f;
	float objectDiffuseB = 1.0f;
	float objectDiffuseA = 1.0f;

	float objectSpecularR = 1.0f;
	float objectSpecularG = 1.0f;
	float objectSpecularB = 1.0f;
	float objectSpecularA = 1.0f;

public:
	Vector3 objectTransform = { objectPosX , objectPosY, objectPosZ };
	Vector2 objectRotate = { objectYaw, objectPitch };
	Vector4 objectAmbient = { objectAmbientR, objectAmbientG, objectAmbientB, objectAmbientA };
	Vector4 objectDiffuse = { objectDiffuseR, objectDiffuseG, objectDiffuseB, objectDiffuseA };
	Vector4 objectSpecular = { objectSpecularR, objectSpecularG, objectSpecularB, objectSpecularA };

	// 빛
	// 위치
	float lightPosX = 0.0f;
	float lightPosY = 0.0f;
	float lightPosZ = 0.0f;

	// 색상
	float ambientColorR = 0.1f;
	float ambientColorG = 0.1f;
	float ambientColorB = 0.1f;
	float ambientColorA = 0.1f;

	float diffuseColorR = 1.0f;
	float diffuseColorG = 1.0f;
	float diffuseColorB = 1.0f;
	float diffuseColorA = 1.0f;

	float specularColorR = 1.0f;
	float specularColorG = 1.0f;
	float specularColorB = 1.0f;
	float specularColorA = 1.0f;

	float shininess = 1000.0f;

	// 방향
	float lightDirX = 0.0f;
	float lightDirY = 0.0f;
	float lightDirZ = 1.0f;

	Vector4 ambientColor = { ambientColorR, ambientColorG, ambientColorB, ambientColorA };
	Vector4 diffuseColor = { diffuseColorR, diffuseColorG, diffuseColorB, diffuseColorA };
	Vector4 specularColor = { specularColorR, specularColorG, specularColorB, specularColorA };
	Vector3 lightDir = { lightDirX, lightDirY, lightDirZ };

	bool viewChanger = false;

	bool m_ControllerWindowOpen = true;

	Vector2 depth = { nearZ, farZ };

private:
	GameApp* m_app;

	void ObjectReset()
	{
		objectScaleXYZ = 200.0f;
		objectTransform = { objectPosX , objectPosY, objectPosZ };
		objectRotate = { 0.0f, 0.0f };
		objectAmbient = { objectAmbientR, objectAmbientG, objectAmbientB, objectAmbientA };
		objectDiffuse = { objectDiffuseR, objectDiffuseG, objectDiffuseB, objectDiffuseA };
		objectSpecular = { objectSpecularR, objectSpecularG, objectSpecularB, objectSpecularA };
	}

	void LightReset()
	{
		ambientColor = { ambientColorR, ambientColorG, ambientColorB, ambientColorA };
		diffuseColor = { diffuseColorR, diffuseColorG, diffuseColorB, diffuseColorA };
		specularColor = { specularColorR, specularColorG, specularColorB, specularColorA };
		lightDir = { lightDirX, lightDirY, lightDirZ };
		shininess = 1000.0f;
	}
};