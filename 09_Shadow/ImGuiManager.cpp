#include "ImGuiManager.h"
#include <string>

#include "TutorialApp.h"

void ImGuiManager::Initialize(ID3D11Device* _Device, ID3D11DeviceContext* _DeviceContext)
{
	LightReset();

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

void ImGuiManager::BeginFrame()
{
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();
}

void ImGuiManager::EndFrame()
{
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}

void ImGuiManager::RenderObjectUI(const char* label, Object& obj)
{
	ImGui::SliderFloat3((std::string(label) + u8" 크기").c_str(), &obj.transform.scale.x, 1.0f, 100.0f);
	ImGui::SliderFloat3((std::string(label) + u8" 위치").c_str(), &obj.transform.position.x, -100.0f, 100.0f, "%.1f");
	ImGui::DragFloat3((std::string(label) + u8" 회전").c_str(), &obj.transform.rotation.x, 0.1f);
	ImGui::ColorEdit4((std::string(label) + u8" Material Ambient").c_str(), &obj.ambient.x);
	ImGui::ColorEdit4((std::string(label) + u8" Material Diffuse").c_str(), &obj.diffuse.x);
	ImGui::ColorEdit4((std::string(label) + u8" Material Specular").c_str(), &obj.specular.x);
	ImGui::SliderFloat((std::string(label) + u8" 광택지수").c_str(), &obj.shininess, 200.0f, 20000.0f);
	if (ImGui::Button((std::string(label) + u8" 초기화").c_str())) obj.Reset();
	ImGui::Text("");
}

void ImGuiManager::Update()
{
    ImGuiIO& io = ImGui::GetIO();

    static ImVec2 lastMousePosBeforeWarp = ImVec2(-1, 1);

    // 드래그 중이 아니면 리셋
    if (!ImGui::IsMouseDown(ImGuiMouseButton_Left))
    {
        lastMousePosBeforeWarp = ImVec2(-1, -1);
        return;
    }

    // 현재 윈도우 핸들 가져오기
    HWND hwnd = GetActiveWindow(); // 또는 메인 윈도우 핸들 저장해두기
    if (!hwnd) return;

    // 현재 마우스 위치 (스크린 좌표)
    POINT screenPos;
    GetCursorPos(&screenPos);

    // 윈도우 클라이언트 영역 정보
    RECT clientRect;
    GetClientRect(hwnd, &clientRect);
    POINT clientOrigin = { 0, 0 };
    ClientToScreen(hwnd, &clientOrigin);

    int clientW = clientRect.right - clientRect.left;
    int clientH = clientRect.bottom - clientRect.top;

    // 클라이언트 영역 기준으로 변환
    int localX = screenPos.x - clientOrigin.x;
    int localY = screenPos.y - clientOrigin.y;

    // 워프 직전 델타 저장
    ImVec2 currentDelta = io.MouseDelta;

    // 경계 체크 및 워프 (여유 공간 10px)
    const int margin = 10;
    bool wrapped = false;
    POINT newScreenPos = screenPos;

    if (localX <= margin)
    {
        newScreenPos.x = clientOrigin.x + clientW - margin - 1;
        wrapped = true;
    }
    else if (localX >= clientW - margin)
    {
        newScreenPos.x = clientOrigin.x + margin + 1;
        wrapped = true;
    }

    if (localY <= margin)
    {
        newScreenPos.y = clientOrigin.y + clientH - margin - 1;
        wrapped = true;
    }
    else if (localY >= clientH - margin)
    {
        newScreenPos.y = clientOrigin.y + margin + 1;
        wrapped = true;
    }

    if (wrapped)
    {
        // 워프 직전 위치 저장
        lastMousePosBeforeWarp = io.MousePos;

        // 커서 워프
        SetCursorPos(newScreenPos.x, newScreenPos.y);

        // ImGui에게 새 위치 알리기 (클라이언트 좌표로)
        io.MousePos = ImVec2(
            (float)(newScreenPos.x - clientOrigin.x),
            (float)(newScreenPos.y - clientOrigin.y)
        );

        // 중요: Delta는 유지! 워프는 화면상 위치만 바꿀 뿐
        // 실제 마우스 이동량은 그대로 전달되어야 함
        io.MouseDelta = currentDelta;

        // WantSetMousePos로 ImGui에게 위치 변경 알림
        io.WantSetMousePos = true;
    }
    else if (lastMousePosBeforeWarp.x >= 0)
    {
        // 워프 직후 첫 프레임
        // 이때 Delta가 비정상적으로 클 수 있으므로 보정
        ImVec2 expectedDelta = ImVec2(
            io.MousePos.x - lastMousePosBeforeWarp.x,
            io.MousePos.y - lastMousePosBeforeWarp.y
        );

        // Delta가 비정상적으로 크면 (화면 반대편으로 점프) 무시
        float deltaLen = sqrtf(expectedDelta.x * expectedDelta.x +
            expectedDelta.y * expectedDelta.y);
        if (deltaLen > 100.0f) // threshold
        {
            io.MouseDelta = ImVec2(0, 0);
        }

        lastMousePosBeforeWarp = ImVec2(-1, -1);
    }
}

void ImGuiManager::Render()
{












	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

void ImGuiManager::DrawShadowSRV(ID3D11ShaderResourceView* shadowSRV, float width, float height)
{
    ImGui::Begin(u8"그림자 맵");
    if (shadowSRV)
    {
        float maxWidth = 256.0f;
        float maxHeight = 256.0f;

        float displayWidth = width;
        float displayHeight = height;

        if (displayWidth > maxWidth)
        {
            float ratio = maxWidth / displayWidth;
            displayWidth = maxWidth;
            displayHeight *= ratio;
        }
        if (displayHeight > maxHeight)
        {
            float ratio = maxHeight / displayHeight;
            displayHeight = maxHeight;
            displayWidth *= ratio;
        }

        ImGui::Image((ImTextureID)shadowSRV, ImVec2(displayWidth, displayHeight));
    }

    ImGui::End();
}

void ImGuiManager::DrawObjectUI()
{
    ImGuiIO& io = ImGui::GetIO(); (void)io;

    //ImGui::ShowDemoWindow();

    //ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);

    ImGui::Begin(u8"컨트롤러");                        // Create a window called "Hello, world!" and append into it.
    //ImGui::Text(u8"TIP : ESC를 누르면 자유롭게 화면을 이동할 수 있습니다.");               // Display some text (you can use a format strings too)
    //ImGui::Text(u8"TIP : 다시 ESC를 눌러 화면 이동을 종료합니다.");
    ImGui::SeparatorText(u8"오브젝트 조절");

    //ImGui::SliderFloat("Camera Distance", &m_PlayerPosZ, 0.0f, 10.0f);            // Edit 1 float using a slider from 0.0f to 1.0f

    //ImGui::Separator();

    // Object1
    RenderObjectUI(u8" 오브젝트1", object1);

    // Object2
    //RenderObjectUI(u8" 오브젝트2", object2);

    // Object3
    //RenderObjectUI(u8" 오브젝트3", object3);

    // ...

    // Light
    ImGui::SeparatorText(u8"빛 조절");
    ImGui::ColorEdit4(u8" Ambient 색상", &ambientLight.x);
    ImGui::ColorEdit4(u8" Diffuse 색상", &diffuseLight.x);
    ImGui::ColorEdit4(u8" Specular 색상", &specularLight.x);
    ImGui::SliderFloat3(u8" 방향", &lightDir.x, -1.0f, 1.0f, "%.1f");

    ImGui::Checkbox("Use Lighting", &useLighting);

    if (ImGui::Button(u8" 빛 초기화"))LightReset();

    ImGui::Text("");

    // 카메라
    ImGui::SeparatorText(u8" 카메라 조절");
    ImGui::SliderFloat(u8" 시야범위", &FOV, 0.6f, 100.0f);
    ImGui::DragFloat2(u8" 렌더 최소/최대 거리", &depth.x, 0.1f, 1.0f, 10000.0f, "%.1f");
    if (depth.x > depth.y) depth.y = depth.x;

    if (ImGui::Button(u8"카메라 초기화"))
    {
        FOV = 1.0f;

        depth.x = 1.0f;
        depth.y = 10000.0f;
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
