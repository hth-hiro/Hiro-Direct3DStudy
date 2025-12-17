#include "pch.h"
#include "GameApp.h"
#include "Helper.h"

#include <dwmapi.h>
#pragma comment(lib, "dwmapi.lib")

GameApp* GameApp::m_pInstance = nullptr;
HWND GameApp::m_hWnd;

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT CALLBACK DefaultWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if (ImGui_ImplWin32_WndProcHandler(hWnd, message, wParam, lParam)) return true;

	return GameApp::m_pInstance->WndProc(hWnd, message, wParam, lParam);
}

GameApp::GameApp(HINSTANCE hInstance)
	:m_hInstance(hInstance), m_szWindowClass(L"DefaultWindowClass"), m_szTitle(L"D3D Class"), m_ClientWidth(1024), m_ClientHeight(768)
{
	GameApp::m_pInstance = this;
	m_wcex.hInstance = hInstance;
	m_wcex.cbSize = sizeof(WNDCLASSEX);
	m_wcex.style = CS_HREDRAW | CS_VREDRAW;
	m_wcex.lpfnWndProc = DefaultWndProc;
	m_wcex.cbClsExtra = 0;
	m_wcex.cbWndExtra = 0;
	m_wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	m_wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	m_wcex.lpszClassName = m_szWindowClass;

    // 아이콘 설정
    m_wcex.hIcon = (HICON)LoadImageW(
        NULL,
        L"..\\Resource\\Icon\\Soline.ico",
        IMAGE_ICON,
        16, 16,              // 아이콘 크기
        LR_LOADFROMFILE
    );
}

GameApp::~GameApp()
{
}

bool GameApp::Initialize(UINT Width, UINT Height)
{
	m_ClientWidth = Width;
	m_ClientHeight = Height;

	RegisterClassExW(&m_wcex);

	RECT rcClient = { 0,0,(LONG)Width, (LONG)Height };
	AdjustWindowRect(&rcClient, WS_OVERLAPPEDWINDOW, FALSE);

    //SetCurrentProcessExplicitAppUserModelID(L"com.gameinjae.icondebug.unique");

	//생성
	m_hWnd = CreateWindowW(m_szWindowClass, m_szTitle, WS_OVERLAPPEDWINDOW,
		100, 100,	// 생성되는 위치
		rcClient.right - rcClient.left, rcClient.bottom - rcClient.top,
		nullptr, nullptr, m_hInstance, nullptr);

	if (!m_hWnd)
	{
		return false;
	}

    // 창 색깔 설정, 글씨 색 설정
    SetTitleBarColor(m_hWnd, ColorF(227,227,232));
    SetTitleTextColor(m_hWnd, ColorF(0, 0, 0));

	ShowWindow(m_hWnd, SW_SHOW);
	UpdateWindow(m_hWnd);

	m_Time.Initialize();
	m_Input.Initialize(m_hWnd,this);

	return true;
}

bool GameApp::Run()
{
    // 런타임에도 창 색 변경 가능
    //SetTitleBarColor(m_hWnd, ColorF(227, 227, 232));
    //SetTitleTextColor(m_hWnd, ColorF(0, 0, 0));

    // 런타임에도 창 아이콘 변경 가능
    //m_wcex.hIcon = (HICON)LoadImageW(
    //    NULL,
    //    L"..\\Resource\\Icon\\Miku.ico",
    //    IMAGE_ICON,
    //    128, 128,           // 아이콘 크기
    //    LR_LOADFROMFILE
    //);

    //SendMessage(m_hWnd, WM_SETICON, ICON_SMALL, (LPARAM)m_wcex.hIcon);

    // 아이콘 설정
    HICON hBig = (HICON)LoadImageW(nullptr, L"..\\Resource\\Icon\\Soline.ico",
        IMAGE_ICON, 128, 128, LR_LOADFROMFILE);
    HICON hSmall = (HICON)LoadImageW(nullptr, L"..\\Resource\\Icon\\Soline.ico",
        IMAGE_ICON, 16, 16, LR_LOADFROMFILE);

    SetClassLongPtrW(m_hWnd, GCLP_HICON, (LONG_PTR)hBig);
    SetClassLongPtrW(m_hWnd, GCLP_HICONSM, (LONG_PTR)hSmall);

    SendMessageW(m_hWnd, WM_SETICON, ICON_BIG, (LPARAM)hBig);
    SendMessageW(m_hWnd, WM_SETICON, ICON_SMALL, (LPARAM)hSmall);

    SetWindowPos(m_hWnd, nullptr, 0, 0, 0, 0,
        SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);

	// Game Loop
	while (TRUE)
	{
		if (PeekMessage(&m_msg, NULL, 0, 0, PM_REMOVE))
		{
			if (m_msg.message == WM_QUIT)
				break;

			TranslateMessage(&m_msg);
			DispatchMessage(&m_msg);
		}
		else
		{
			Update();
			Render();
		}
	}

	return 0;
}

void GameApp::Update()
{
	float deltaTime = m_Time.GetDeltaTime();
	m_Time.Update();
	m_Input.Update(deltaTime);
	m_Camera.Update(deltaTime);
}

void GameApp::OnInputProcess(const Keyboard::State& KeyState, const Keyboard::KeyboardStateTracker& KeyTracker, const Mouse::State& MouseState, const Mouse::ButtonStateTracker& MouseTracker)
{
	m_Camera.OnInputProcess(KeyState, KeyTracker, MouseState, MouseTracker);
}

LRESULT GameApp::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_ACTIVATEAPP:
		Keyboard::ProcessMessage(message, wParam, lParam);
		Mouse::ProcessMessage(message, wParam, lParam);
		break;
	case WM_INPUT:
	case WM_MOUSEMOVE:
	case WM_LBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_RBUTTONDOWN:
	case WM_RBUTTONUP:
	case WM_MBUTTONDOWN:
	case WM_MBUTTONUP:
	case WM_MOUSEWHEEL:
	case WM_XBUTTONDOWN:
	case WM_XBUTTONUP:
	case WM_MOUSEHOVER:
		Mouse::ProcessMessage(message, wParam, lParam);
		break;
	case WM_KEYDOWN:
	case WM_KEYUP:
	case WM_SYSKEYUP:
		Keyboard::ProcessMessage(message, wParam, lParam);
		break;

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}

	return 0;
}

void GameApp::SetTitleBarColor(HWND hWnd, ColorF color)
{
    const DWORD DWMWA_USE_IMMERSIVE_DARK_MODE = 20; // Win10 2004+/Win11
    const DWORD DWMWA_USE_IMMERSIVE_DARK_MODE_OLD = 19; // Win10 1809~1903
    const DWORD DWMWA_CAPTION_COLOR = 35; // Win11
    const DWORD DWMWA_TEXT_COLOR = 36; // Win11

    //BOOL on = enable ? TRUE : FALSE;
    BOOL on = TRUE;
    
    // 다크 타이틀바 적용
    if (FAILED(DwmSetWindowAttribute(hWnd, DWMWA_USE_IMMERSIVE_DARK_MODE, &on, sizeof(on))))
        DwmSetWindowAttribute(hWnd, DWMWA_USE_IMMERSIVE_DARK_MODE_OLD, &on, sizeof(on));

    // Win11이면 캡션/텍스트 색까지 강제(완전 검정/흰색)
    COLORREF caption = RGB(color.r, color.g, color.b); // 원하는 타이틀 바 색
    COLORREF text = RGB(255, 255, 255);  // 타이틀 글자 색
    DwmSetWindowAttribute(hWnd, DWMWA_CAPTION_COLOR, &caption, sizeof(caption));
    //DwmSetWindowAttribute(hWnd, DWMWA_TEXT_COLOR, &text, sizeof(text));
}

void GameApp::SetTitleTextColor(HWND hWnd, ColorF color)
{
    COLORREF text = RGB(color.r, color.g, color.b);  // 타이틀 글자 색
    DwmSetWindowAttribute(hWnd, DWMWA_TEXT_COLOR, &text, sizeof(text));
}
