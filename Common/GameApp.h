#pragma once
#include <windows.h>
#include <imgui.h>

#include "TimeSystem.h"
#include "InputSystem.h"
#include "Camera.h"

#include "Define.h"

#define MAX_LOADSTRING 100

struct ColorF
{
    ColorF(int _r, int _g, int _b) : r(_r), g(_g), b(_b) {}

    int r;
    int g;
    int b;
};

class GameApp : public InputProcesser
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

	TimeSystem	m_Time;
	InputSystem	m_Input;
	Camera		m_Camera;

public:
	virtual bool Initialize(UINT Width, UINT Height);
	virtual bool Run();
	virtual void Update();
	virtual void Render() = 0;

	virtual void OnInputProcess(const Keyboard::State& KeyState, const Keyboard::KeyboardStateTracker& KeyTracker,
		const Mouse::State& MouseState, const Mouse::ButtonStateTracker& MouseTracker);

	virtual LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

    void SetTitleBarColor(HWND hWnd, ColorF color);
    void SetTitleTextColor(HWND hWnd, ColorF color);
};