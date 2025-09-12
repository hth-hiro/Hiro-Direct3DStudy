#include "pch.h"
#include "GameApp.h"

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

	//积己
	m_hWnd = CreateWindowW(m_szWindowClass, m_szTitle, WS_OVERLAPPEDWINDOW,
		100, 100,	// 积己登绰 困摹
		rcClient.right - rcClient.left, rcClient.bottom - rcClient.top,
		nullptr, nullptr, m_hInstance, nullptr);

	if (!m_hWnd)
	{
		return false;
	}

	ShowWindow(m_hWnd, SW_SHOW);
	UpdateWindow(m_hWnd);

	return true;
}

bool GameApp::Run()
{
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

}


LRESULT GameApp::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}

	return 0;
}
