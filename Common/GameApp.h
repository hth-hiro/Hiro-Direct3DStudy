#pragma once
#include <windows.h>
#include <imgui.h>

#include "TimeSystem.h"

#define MAX_LOADSTRING 100

class GameApp
{
public:
	GameApp(HINSTANCE hInstace);
	virtual ~GameApp();

	static HWND m_hWnd;
	static GameApp* m_pInstance;

public:
	HACCEL m_hAccelTable;
	MSG m_msg;
	HINSTANCE m_hInstance;
	WCHAR m_szTitle[MAX_LOADSTRING];			// √¢ ¿Ã∏ß
	WCHAR m_szWindowClass[MAX_LOADSTRING];
	WNDCLASSEXW m_wcex;
	
	UINT m_ClientWidth;
	UINT m_ClientHeight;

	TimeSystem m_Time;

public:
	virtual bool Initialize(UINT Width, UINT Height);
	virtual bool Run();
	virtual void Update();
	virtual void Render() = 0;
	virtual LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
};