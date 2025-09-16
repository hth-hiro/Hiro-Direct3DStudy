#pragma once
#include <Windows.h>
#include <directxtk/Mouse.h>
#include <directxtk/Keyboard.h>

using namespace DirectX;

class InputProcesser
{
public:
	virtual void OnInputProcess(const Keyboard::State& KeyState, const Keyboard::KeyboardStateTracker& KeyTracker,
		const Mouse::State& MouseState, const Mouse::ButtonStateTracker& MouseTracker) = 0;
};

class InputSystem
{
public:
	InputSystem();
	~InputSystem() {};

	static InputSystem* Instance;

	InputProcesser* m_pInputProcessers = nullptr;

	// input
	std::unique_ptr<Keyboard>			m_Keyboard;
	std::unique_ptr<Mouse>				m_Mouse;
	Keyboard::KeyboardStateTracker		m_KeyboardStateTracker;
	Mouse::ButtonStateTracker			m_MouseStateTracker;

	Mouse::State						m_MouseState;
	Keyboard::State						m_KeyboardState;

	void Update(float DeltaTime);
	bool Initialize(HWND hWnd, InputProcesser* processer);
};

