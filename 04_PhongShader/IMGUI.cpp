#include "IMGUI.h"

void IMGUI::Initialize(ID3D11Device* _Device, ID3D11DeviceContext* _DeviceContext)
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	// GUI의 테마를 결정한다.
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsLight();
	//ImGui::StyleColorsClassic();

	ImGui_ImplWin32_Init(GameApp::m_hWnd);
	ImGui_ImplDX11_Init(_Device, _DeviceContext);

	ImGuiIO& io = ImGui::GetIO(); (void)io;

	io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\Malgun.ttf", 20.0f, NULL, io.Fonts->GetGlyphRangesKorean());
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
}

void IMGUI::Render()
{
	ImGuiIO& io = ImGui::GetIO(); (void)io;

	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	//ImGui::ShowDemoWindow();

	//ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);

	{
		ImGui::Begin(u8"컨트롤러");                        // Create a window called "Hello, world!" and append into it.
		ImGui::Text(u8"TIP : ESC를 누르면 자유롭게 화면을 이동할 수 있습니다.");               // Display some text (you can use a format strings too)
		ImGui::Text(u8"TIP : 다시 ESC를 눌러 화면 이동을 종료합니다.");
		ImGui::SeparatorText(u8"오브젝트 조절");

		//ImGui::SliderFloat("Camera Distance", &m_PlayerPosZ, 0.0f, 10.0f);            // Edit 1 float using a slider from 0.0f to 1.0f

		//ImGui::Separator();

		// Object
		Vector3 objectPos = { objectPosX , objectPosY, objectPosZ };
		Vector2 objectRotate = { objectYaw, objectPitch };
		Vector4 objectAmbient = { objectAmbientR, objectAmbientG, objectAmbientB, objectAmbientA };
		Vector4 objectDiffuse = { objectDiffuseR, objectDiffuseG, objectDiffuseB, objectDiffuseA };
		Vector4 objectSpecular = { objectSpecularR, objectSpecularG, objectSpecularB, objectSpecularA };

		ImGui::SliderFloat(u8" 크기", &objectScaleXYZ, 10.0f, 1000.0f);
		ImGui::SliderFloat3(u8" 위치", &objectPos.x, -10.0f, 10.0f);
		ImGui::DragFloat2(u8"Y축 회전 & X축 회전", &objectRotate.x, 0.1f);
		ImGui::ColorEdit4(u8" Material Ambient 색상", &objectAmbient.x);
		ImGui::ColorEdit4(u8" Material Diffuse 색상", &objectDiffuse.x);
		ImGui::ColorEdit4(u8" Material Specular 색상", &objectSpecular.x);

		if (ImGui::Button(u8"오브젝트 초기화"))
		{
			objectScaleXYZ = 200.0f;

			objectPosX = 0.0f;
			objectPosY = 0.0f;
			objectPosZ = 0.0f;

			objectYaw = 0.0f;
			objectPitch = 0.0f;

			objectAmbientR = 0.1f;
			objectAmbientG = 0.1f;
			objectAmbientB = 0.1f;
			objectAmbientA = 0.1f;

			objectDiffuseR = 1.0f;
			objectDiffuseG = 1.0f;
			objectDiffuseB = 1.0f;
			objectDiffuseA = 1.0f;

			objectSpecularR = 1.0f;
			objectSpecularG = 1.0f;
			objectSpecularB = 1.0f;
			objectSpecularA = 1.0f;
		}
		ImGui::Text("");

		// Light
		Vector4 ambientColor = { ambientColorR, ambientColorG, ambientColorB, ambientColorA };
		Vector4 diffuseColor = { diffuseColorR, diffuseColorG, diffuseColorB, diffuseColorA };
		Vector4 specularColor = { specularColorR, specularColorG, specularColorB, specularColorA };
		Vector3 lightDir = { lightDirX, lightDirY, lightDirZ };

		ImGui::SeparatorText(u8"빛 조절");
		ImGui::ColorEdit4(u8" Ambient 색상", &ambientColor.x);
		ImGui::ColorEdit4(u8" Diffuse 색상", &diffuseColor.x);
		ImGui::ColorEdit4(u8" Specular 색상", &specularColor.x);
		ImGui::SliderFloat(u8" 광택지수", &shininess, 200.0f, 20000.0f);
		ImGui::SliderFloat3(u8" 방향", &lightDir.x, -1.0f, 1.0f);

		if (ImGui::Button(u8" 빛 초기화"))
		{
			ambientColorR = 0.1f;
			ambientColorG = 0.1f;
			ambientColorB = 0.1f;

			diffuseColorR = 1.0f;
			diffuseColorG = 1.0f;
			diffuseColorB = 1.0f;

			specularColorR = 1.0f;
			specularColorG = 1.0f;
			specularColorB = 1.0f;

			lightDirX = 0.0f;
			lightDirY = 0.0;
			lightDirZ = 1.0;

			shininess = 1000.0f;
		}
		ImGui::Text("");

		// 카메라
		Vector2 depth = { nearZ, farZ };
		ImGui::SeparatorText(u8" 카메라 조절");
		ImGui::SliderFloat(u8" 시야범위", &FOV, 0.6f, 10.0f);
		ImGui::DragFloat2(u8" 렌더 최소/최대 거리", &depth.x, 0.1f, 0.01f, 50000.0f, "%.1f");
		if (nearZ > farZ) farZ = nearZ;

		if (ImGui::Button(u8"카메라 초기화"))
		{
			FOV = 1.0f;

			nearZ = 0.01f;
			farZ = 50000.0f;
		}
		ImGui::Text("");

		// 스카이박스
		ImGui::SeparatorText(u8" 배경 선택");
		if (ImGui::Button(u8"실내"))		viewChanger = false; ImGui::SameLine();
		if (ImGui::Button(u8"디버그"))	viewChanger = true;
		ImGui::Text("");

		// 디버그
		ImGui::Separator();
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);

		ImGui::End();
	}

	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

void IMGUI::Release()
{
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}