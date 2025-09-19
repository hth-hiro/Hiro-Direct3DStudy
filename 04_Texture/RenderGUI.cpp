#include "RenderGUI.h"

void RenderGUI::Initialize(ID3D11Device* _Device, ID3D11DeviceContext* _DeviceContext)
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	// GUI�� �׸��� �����Ѵ�.
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

void RenderGUI::Render()
{
	ImGuiIO& io = ImGui::GetIO(); (void)io;

	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	//ImGui::ShowDemoWindow();

	//ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);

	{
		ImGui::Begin(u8"��Ʈ�ѷ�");                        // Create a window called "Hello, world!" and append into it.
		//ImGui::Text("Controller");               // Display some text (you can use a format strings too)
		//ImGui::Text("");
		ImGui::SeparatorText(u8"������Ʈ ����");

		//ImGui::SliderFloat("Camera Distance", &m_PlayerPosZ, 0.0f, 10.0f);            // Edit 1 float using a slider from 0.0f to 1.0f

		//ImGui::Separator();

		// Object
		float objectPos[3] = { objectPosX , objectPosY, objectPosZ };

		if (ImGui::SliderFloat3(u8" ��ġ", objectPos, -10.0f, 10.0f))
		{
			objectPosX = objectPos[0];
			objectPosY = objectPos[1];
			objectPosZ = objectPos[2];
		}

		float objectRotate[2] = { objectYaw, objectPitch };

		if (ImGui::DragFloat2(u8"Y�� ȸ�� & X�� ȸ��", objectRotate, 0.1f))
		{
			objectYaw = objectRotate[0];
			objectPitch = objectRotate[1];
		}

		if (ImGui::Button(u8"������Ʈ �ʱ�ȭ"))
		{
			objectPosX = 0.0f;
			objectPosY = 0.0f;
			objectPosZ = 0.0f;

			objectYaw = 45.0f;
			objectPitch = 0.0f;
		}

		ImGui::Text("");

		// Light
		ImGui::SeparatorText(u8"�� ����");
		//float lightPos[3] = { lightPosX, lightPosY, lightPosZ };

		//if (ImGui::SliderFloat3(" LightPos", lightPos, -10.0f, 10.0f))
		//{
		//	lightPosX = lightPos[0];
		//	lightPosY = lightPos[1];
		//	lightPosZ = lightPos[2];
		//}

		float lightColor[3] = { lightColorR, lightColorG, lightColorB };

		if (ImGui::ColorEdit3(u8" ����", lightColor))
		{
			lightColorR = lightColor[0];
			lightColorG = lightColor[1];
			lightColorB = lightColor[2];
		}

		float lightDir[3] = { lightDirX, lightDirY, lightDirZ };

		if (ImGui::DragFloat3(u8" ����", lightDir, 0.1f, -1.0f, 1.0f))
		{
			lightDirX = lightDir[0];
			lightDirY = lightDir[1];
			lightDirZ = lightDir[2];
			lightDirZ = lightDir[2];
		}

		if (ImGui::Button(u8" �� �ʱ�ȭ"))
		{
			//lightPosX = 0.0f;
			//lightPosY = 0.0f;
			//lightPosZ = 0.0f

			lightColorR = 1.0f;
			lightColorG = 1.0f;
			lightColorB = 1.0f;

			lightDirX = -0.577f;
			lightDirY = 0.577f;
			lightDirZ = -0.577f;
		}

		ImGui::Text("");

		ImGui::SeparatorText(u8" ī�޶� ����");

		//float cameraPosX = m_Camera.GetPosition().x;
		//float cameraPosY = m_Camera.GetPosition().y;
		//float cameraPosZ = m_Camera.GetPosition().z;

		//float cameraPos[3] = { cameraPosX, cameraPosY, cameraPosZ };

		//// �÷��̾� ��ġ ������ �����Ӱ� ���� ����
		//if (ImGui::DragFloat3(" CameraPos", cameraPos, 0.1f))
		//{
		//	cameraPosX = cameraPos[0];
		//	cameraPosY = cameraPos[1];
		//	cameraPosY = cameraPos[2];
		//}

		ImGui::SliderFloat(u8" �þ߹���", &FOV, 0.6f, 10.0f);

		float depth[2] = { nearZ, farZ };

		if (ImGui::DragFloat2(u8" ���� �ּ�/�ִ� �Ÿ�", depth, 0.1f, 0.01f, 1000.0f, "%.1f"))
		{
			nearZ = depth[0];
			farZ = depth[1];

			if (nearZ > farZ)
			{
				farZ = nearZ;
			}
		}

		if (ImGui::Button(u8"ī�޶� �ʱ�ȭ"))
		{
			FOV = 1.0f;

			nearZ = 0.01f;
			farZ = 100.0f;
		}

		ImGui::Text("");
		ImGui::SeparatorText(u8" ��� ����");

		if (ImGui::Button(u8"�ǳ�"))
		{
			viewChanger = false;
		}

		ImGui::SameLine();
		if (ImGui::Button(u8"�ǿ�"))
		{
			viewChanger = true;
		}

		ImGui::Text("");

		ImGui::Separator();
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