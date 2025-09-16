#include "pch.h"
#include "Camera.h"

Camera::Camera()
{
    Reset();
}

Camera::~Camera()
{
	InputSystem::Instance->m_Mouse->SetMode(Mouse::MODE_ABSOLUTE);
}

void Camera::Reset()
{
    m_World = Matrix::Identity;
    m_Rotation = Vector3(0.0f, 0.0f, 0.0f);
    m_Position = Vector3(0.0f, 0.0f, -10.0f);
}

void Camera::Update(float deltaTime)
{
    if (m_InputVector.Length() > 0.0f)
    {
		m_Position += m_InputVector * m_MoveSpeed * deltaTime;
        m_InputVector = Vector3(0.0f, 0.0f, 0.0f);
    }

    m_World = Matrix::CreateFromYawPitchRoll(m_Rotation) *
        Matrix::CreateTranslation(m_Position);
}

void Camera::GetViewMatrix(Matrix& out)
{
    Vector3 eye = m_World.Translation();
    Vector3 target = m_World.Translation() + GetForward();
    Vector3 up = m_World.Up();
    out = XMMatrixLookAtLH(eye, target, up);
}

Vector3 Camera::GetForward()
{
    return -m_World.Forward();
}

Vector3 Camera::GetRight()
{
    return m_World.Right();
}

void Camera::AddInputVector(const Vector3& input)
{
    m_InputVector += input;
    m_InputVector.Normalize();
}

void Camera::AddPitch(float value)
{
    m_Rotation.x += value;

    // 안정성 고려 크기 제한 (상하)
    if (m_Rotation.x > XM_PIDIV2)  m_Rotation.x = XM_PIDIV2;
    if (m_Rotation.x < -XM_PIDIV2) m_Rotation.x = -XM_PIDIV2;
}

void Camera::AddYaw(float value)
{
    m_Rotation.y += value;
}

void Camera::OnInputProcess(const Keyboard::State& KeyState, const Keyboard::KeyboardStateTracker& KeyTracker, const Mouse::State& MouseState, const Mouse::ButtonStateTracker& MouseTracker)
{
    Vector3 forward = GetForward();
    Vector3 right = GetRight();

	//delta = Vector3(float(MouseState.x), float(MouseState.y), 0.0f);

	if (KeyTracker.IsKeyPressed(Keyboard::Keys::R))
	{
		Reset();
	}

	if (KeyState.IsKeyDown(Keyboard::Keys::W))
	{
		AddInputVector(forward);
	}
	else if (KeyState.IsKeyDown(Keyboard::Keys::S))
	{
		AddInputVector(-forward);
	}

	if (KeyState.IsKeyDown(Keyboard::Keys::A))
	{
		AddInputVector(-right);
	}
	else if (KeyState.IsKeyDown(Keyboard::Keys::D))
	{
		AddInputVector(right);
	}

	if (KeyState.IsKeyDown(Keyboard::Keys::LeftShift) || KeyState.IsKeyDown(Keyboard::Keys::E))
	{
		AddInputVector(-m_World.Up());
	}
	else if (KeyState.IsKeyDown(Keyboard::Keys::Space) || KeyState.IsKeyDown(Keyboard::Keys::Q))
	{
		AddInputVector(m_World.Up());
	}

	if (KeyTracker.IsKeyPressed(Keyboard::Keys::Escape))
	{
		if (isFPSMode)
		{
			InputSystem::Instance->m_Mouse->SetMode(Mouse::MODE_ABSOLUTE);
			isFPSMode = false;
		}
		else
		{
			InputSystem::Instance->m_Mouse->SetMode(Mouse::MODE_RELATIVE);
			isFPSMode = true;
		}
	}

	// 나중에 쓸수도 있으니 보류, 현재 교수님 코드 기준으로 반영
	//if (isFPSMode)
	//{
	//	InputSystem::Instance->m_Mouse->SetMode(Mouse::MODE_RELATIVE);
	//}

	//if (MouseState.positionMode == Mouse::MODE_RELATIVE)
	//{
	//	// 헷갈리지 않게 주의
	//	// Pitch는 X축 기반 회전 -> 카메라 기준 상하
	//	// Yaw는 Y축 기반 회전 -> 카메라 기준 좌우  

	//	Vector3 delta = Vector3(float(MouseState.x), float(MouseState.y), 0.f) * m_RotationSpeed;
	//	AddPitch(delta.y);
	//	AddYaw(delta.x);
	//}

	//if (KeyState.IsKeyDown(Keyboard::Keys::LeftShift))
	//{
	//	// 월드 좌표계로 상하이동
	//	AddInputVector(Vector3(0.0f, -1.0f, 0.0f));
	//}
	//else if (KeyState.IsKeyDown(Keyboard::Keys::Space))
	//{
	//	// 월드 좌표계로 상하이동
	//	AddInputVector(Vector3(0.0f, 1.0f, 0.0f));
	//}

	InputSystem::Instance->m_Mouse->SetMode(MouseState.rightButton ? Mouse::MODE_RELATIVE : Mouse::MODE_ABSOLUTE);
	if (MouseState.positionMode == Mouse::MODE_RELATIVE)
	{
		Vector3 delta = Vector3(float(MouseState.x), float(MouseState.y), 0.f) * m_RotationSpeed;
		AddPitch(delta.y);
		AddYaw(delta.x);
	}
}
