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
		float objectPos[3] = { m_ObjectPosX , m_ObjectPosY, m_ObjectPosZ };

		if (ImGui::SliderFloat3(" ParentPos", objectPos, -10.0f, 10.0f))
		{
			m_ObjectPosX = objectPos[0];
			m_ObjectPosY = objectPos[1];
			m_ObjectPosZ = objectPos[2];
		}

		// Light
		float lightPos[3] = { m_LightPosX, m_LightPosY, m_LightPosZ };

		if (ImGui::SliderFloat3(" Child1Pos", lightPos, -10.0f, 10.0f))
		{
			m_LightPosX = lightPos[0];
			m_LightPosY = lightPos[1];
			m_LightPosZ = lightPos[2];
		}

		if (ImGui::Button("Object Reset"))
		{
			m_ObjectPosX = 0.0f;
			m_ObjectPosY = 0.0f;
			m_ObjectPosZ = 0.0f;

			m_LightPosX = 0.0f;
			m_LightPosY = 0.0f;
			m_LightPosZ = 0.0f;
		}

		ImGui::Text("");

		ImGui::SeparatorText(" Camera Control");

		float playerPos[3] = { m_PlayerPosX, m_PlayerPosY, m_PlayerPosZ };

		// 플레이어 위치 조정은 자유롭게 조절 가능
		if (ImGui::DragFloat3(" CameraPos", playerPos))
		{
			m_PlayerPosX = playerPos[0];
			m_PlayerPosY = playerPos[1];
			m_PlayerPosZ = playerPos[2];
		}

		ImGui::SliderFloat(" FOV", &m_FOV, 0.6f, 10.0f);

		float depth[2] = { m_NearZ, m_FarZ };

		if (ImGui::DragFloat2(" Near & Far", depth, 1.0f, 0.01f, 1000.0f))
		{
			m_NearZ = depth[0];
			m_FarZ = depth[1];

			if (m_NearZ > m_FarZ)
			{
				m_FarZ = m_NearZ;
			}
		}

		if (ImGui::Button("Camera Reset"))
		{
			m_PlayerPosX = 0.0f;
			m_PlayerPosY = 0.0f;
			m_PlayerPosZ = -10.0f;

			m_FOV = 1.0f;

			m_NearZ = 0.01f;
			m_FarZ = 100.0f;

			m_FocusParent = false;

			m_counter = 0;
		}

		ImGui::Text("");
		ImGui::Separator();

		if (ImGui::CollapsingHeader("Dummy"))
		{
			ImGui::Checkbox(" Focus Parent", &m_FocusParent);
			if (ImGui::Button("Click"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
				m_counter++;

			ImGui::SameLine();
			ImGui::Text("counter = %d", m_counter);
		}

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
