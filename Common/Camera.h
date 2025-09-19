#pragma once
#include <directxtk/SimpleMath.h>
#include "InputSystem.h"

using namespace DirectX::SimpleMath;

template <typename T>
T clamp(T value, T minVal, T maxVal)
{
	if (value < minVal) return minVal;
	if (value > maxVal) return maxVal;

	return value;
}

class Camera : public InputProcesser
{
public:
	Camera();
	~Camera();

	void Reset();
	void Update(float deltaTime);
	void GetViewMatrix(Matrix& out);
	XMMATRIX GetViewMatrixNoTranslation(Matrix& out);

	Vector3 GetForward();
	Vector3 GetRight();
	Vector3 GetPosition() const { return m_Position; }

	Vector3 GetForwardYaw();
	void SetSpeed(float speed) { m_MoveSpeed = speed; }

	void AddInputVector(const Vector3& input);
	void AddPitch(float value);
	void AddYaw(float value);

	Matrix GetWorld() { return m_World; }

	bool isFPSMode = false;

private:
	Vector3 m_Rotation;
	Vector3 m_Position;

	Matrix m_World;
	Vector3 m_InputVector;

	float m_MoveSpeed = 20.0f;
	float m_RotationSpeed = 0.004f;	// rad per sec, 회전 감도

	int prevScroll = 0; // 이전 프레임 휠 값 저장


public:
	virtual void OnInputProcess(
		const Keyboard::State& KeyState,
		const Keyboard::KeyboardStateTracker& KeyTracker,
		const Mouse::State& MouseState,
		const Mouse::ButtonStateTracker& MouseTracker
	) override;
};

