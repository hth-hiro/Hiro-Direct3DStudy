#include "ImGuiManager.h"

void ImGuiManager::Initialize()
{
	ObjectReset();
	LightReset();
}

void ImGuiManager::BeginFrame(ID3D11Device* _Device, ID3D11DeviceContext* _DeviceContext)
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

void ImGuiManager::EndFrame()
{
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}

void ImGuiManager::Render()
{
	ImGuiIO& io = ImGui::GetIO(); (void)io;

	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	//ImGui::ShowDemoWindow();

	//ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);

	{
		ImGui::Begin(u8"컨트롤러");                        // Create a window called "Hello, world!" and append into it.
		//ImGui::Text(u8"TIP : ESC를 누르면 자유롭게 화면을 이동할 수 있습니다.");               // Display some text (you can use a format strings too)
		//ImGui::Text(u8"TIP : 다시 ESC를 눌러 화면 이동을 종료합니다.");
		ImGui::SeparatorText(u8"오브젝트 조절");

		//ImGui::SliderFloat("Camera Distance", &m_PlayerPosZ, 0.0f, 10.0f);            // Edit 1 float using a slider from 0.0f to 1.0f

		//ImGui::Separator();

		// Object
		ImGui::SliderFloat(u8" 크기", &objectScaleXYZ, 10.0f, 1000.0f);
		ImGui::SliderFloat3(u8" 위치", &objectTransform.x, -10.0f, 10.0f);
		ImGui::DragFloat2(u8"Y축 회전 & X축 회전", &objectRotate.x, 0.1f);
		ImGui::ColorEdit4(u8" Material Ambient 색상", &objectAmbient.x);
		ImGui::ColorEdit4(u8" Material Diffuse 색상", &objectDiffuse.x);
		ImGui::ColorEdit4(u8" Material Specular 색상", &objectSpecular.x);

		if (ImGui::Button(u8"오브젝트 초기화")) ObjectReset();
		ImGui::Text("");

		// Light
		ImGui::SeparatorText(u8"빛 조절");
		ImGui::ColorEdit4(u8" Ambient 색상", &ambientColor.x);
		ImGui::ColorEdit4(u8" Diffuse 색상", &diffuseColor.x);
		ImGui::ColorEdit4(u8" Specular 색상", &specularColor.x);
		ImGui::SliderFloat(u8" 광택지수", &shininess, 200.0f, 20000.0f);
		ImGui::SliderFloat3(u8" 방향", &lightDir.x, -1.0f, 1.0f);

		ImGui::Checkbox("Use Lighting", &useLighting);

		if (ImGui::Button(u8" 빛 초기화"))
		{
			LightReset();
		}
		ImGui::Text("");

		// 카메라
		ImGui::SeparatorText(u8" 카메라 조절");
		ImGui::SliderFloat(u8" 시야범위", &FOV, 0.6f, 100.0f);
		ImGui::DragFloat2(u8" 렌더 최소/최대 거리", &depth.x, 0.1f, 1.0f, 10000.0f, "%.1f");
		if (nearZ > farZ) farZ = nearZ;

		if (ImGui::Button(u8"카메라 초기화"))
		{
			FOV = 1.0f;

			nearZ = 1.0f;
			farZ = 10000.0f;
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
