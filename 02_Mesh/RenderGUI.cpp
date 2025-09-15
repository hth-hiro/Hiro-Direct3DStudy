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

		// Parent
		float parentWorldPos[3] = { m_ParentWorldX , m_ParentWorldY, m_ParentWorldZ };

		if (ImGui::SliderFloat3(" ParentPos", parentWorldPos, -10.0f, 10.0f))
		{
			m_ParentWorldX = parentWorldPos[0];
			m_ParentWorldY = parentWorldPos[1];
			m_ParentWorldZ = parentWorldPos[2];
		}

		// Child1
		float child1LocalPos[3] = { m_Child1LocalX, m_Child1LocalY, m_Child1LocalZ };

		if (ImGui::SliderFloat3(" Child1Pos", child1LocalPos, -10.0f, 10.0f))
		{
			m_Child1LocalX = child1LocalPos[0];
			m_Child1LocalY = child1LocalPos[1];
			m_Child1LocalZ = child1LocalPos[2];
		}

		// Child2
		float child2LocalPos[3] = { m_Child2LocalX, m_Child2LocalY, m_Child2LocalZ };

		if (ImGui::SliderFloat3(" Child2Pos", child2LocalPos, -10.0f, 10.0f))
		{
			m_Child2LocalX = child2LocalPos[0];
			m_Child2LocalY = child2LocalPos[1];
			m_Child2LocalZ = child2LocalPos[2];
		}

		if (ImGui::Button("Object Reset"))
		{
			m_ParentWorldX = 0.0f;
			m_ParentWorldY = 0.0f;
			m_ParentWorldZ = 0.0f;

			m_Child1LocalX = 8.0f;
			m_Child1LocalY = 0.0f;
			m_Child1LocalZ = 0.0f;

			m_Child2LocalX = 3.0f;
			m_Child2LocalY = 0.0f;
			m_Child2LocalZ = 0.0f;
		}

		ImGui::Text("");

		ImGui::SeparatorText(" Camera Control");

		float playerPos[3] = { playerPosX, playerPosY, playerPosZ };

		//if (ImGui::SliderFloat3("PlayerPos", playerPos, -10.0f, 10.0f))
		//{
		//	m_PlayerPosX = playerPos[0];
		//	m_PlayerPosY = playerPos[1];
		//	m_PlayerPosZ = playerPos[2];
		//}

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

			m_counter = 0;
		}

		ImGui::Text("");
		ImGui::Separator();

		if (ImGui::CollapsingHeader("Dummy"))
		{
			ImGui::Checkbox(" Focus Parent", &isFocusParent);
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
