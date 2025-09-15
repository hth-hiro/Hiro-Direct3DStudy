#include "RenderGUI.h"

void RenderGUI::Initialize(ID3D11Device* _Device, ID3D11DeviceContext* _DeviceContext)
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	// GUI의 테마를 결정한다.
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsLight();
	//ImGui::StyleColorsClassic();

	ImGui_ImplWin32_Init(m_hWnd);
	ImGui_ImplDX11_Init(_Device, _DeviceContext);
}

void RenderGUI::Render()
{
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	//ImGui::ShowDemoWindow();

	{
		ImGui::Begin("Controller");                          // Create a window called "Hello, world!" and append into it.

		//ImGui::Text("Controller");               // Display some text (you can use a format strings too)
		ImGui::Text("");
		ImGui::SeparatorText("Object Control");

		//ImGui::SliderFloat("Camera Distance", &m_PlayerPosZ, 0.0f, 10.0f);            // Edit 1 float using a slider from 0.0f to 1.0f

		ImGui::Separator();

		// Object
		float objectPos[3] = { objectPosX , objectPosY, objectPosZ };

		if (ImGui::SliderFloat3(" ObjectPos", objectPos, -10.0f, 10.0f))
		{
			objectPosX = objectPos[0];
			objectPosY = objectPos[1];
			objectPosZ = objectPos[2];
		}

		// Light
		float lightPos[3] = { lightPosX, lightPosY, lightPosZ };

		if (ImGui::SliderFloat3(" LightPos", lightPos, -10.0f, 10.0f))
		{
			lightPosX = lightPos[0];
			lightPosY = lightPos[1];
			lightPosZ = lightPos[2];
		}

		float lightDir[3] = {};

		if (ImGui::Button("Object Reset"))
		{
			objectPosX = 0.0f;
			objectPosY = 0.0f;
			objectPosZ = 0.0f;

			lightPosX = 0.0f;
			lightPosY = 0.0f;
			lightPosZ = 0.0f;
		}

		ImGui::Text("");

		ImGui::SeparatorText(" Camera Control");

		float playerPos[3] = { playerPosX, playerPosY, playerPosZ };

		// 플레이어 위치 조정은 자유롭게 조절 가능
		if (ImGui::DragFloat3(" CameraPos", playerPos))
		{
			playerPosX = playerPos[0];
			playerPosY = playerPos[1];
			playerPosZ = playerPos[2];
		}

		ImGui::SliderFloat(" FOV", &FOV, 0.6f, 10.0f);

		float depth[2] = { nearZ, farZ };

		if (ImGui::DragFloat2(" Near & Far", depth, 1.0f, 0.01f, 1000.0f))
		{
			nearZ = depth[0];
			farZ = depth[1];

			if (nearZ > farZ)
			{
				farZ = nearZ;
			}
		}

		if (ImGui::Button("Camera Reset"))
		{
			playerPosX = 0.0f;
			playerPosY = 0.0f;
			playerPosZ = -10.0f;

			FOV = 1.0f;

			nearZ = 0.01f;
			farZ = 100.0f;

			isFocusParent = false;
		}

		ImGui::Text("");
		ImGui::SeparatorText("[Debug]");
		ImGui::Checkbox(" Focus Parent", &isFocusParent);

		ImGui::Separator();

		ImGui::Text("");
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);

		ImGui::End();
	}

	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

void RenderGUI::Release()
{
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}
