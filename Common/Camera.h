#pragma once
#include <directxtk/SimpleMath.h>
#include "InputSystem.h"

using namespace DirectX::SimpleMath;

class Camera : public InputProcesser
{
public:
	Camera();
	~Camera();

	void Reset();
	void Update(float deltaTime);
	void GetViewMatrix(Matrix& out);

	Vector3 GetForward();
	Vector3 GetRight();
	Vector3 GetPosition() const { return m_Position; }

	void AddInputVector(const Vector3& input);
	void AddPitch(float value);
	void AddYaw(float value);

private:
	Vector3 m_Rotation;
	Vector3 m_Position;

	Matrix m_World;
	Vector3 m_InputVector;

	float m_MoveSpeed = 20.0f;
	float m_RotationSpeed = 0.004f;	// rad per sec

	bool isFPSMode = false;
	//Vector3 delta = Vector3(0, 0, 0);

public:
	virtual void OnInputProcess(
		const Keyboard::State& KeyState,
		const Keyboard::KeyboardStateTracker& KeyTracker,
		const Mouse::State& MouseState,
		const Mouse::ButtonStateTracker& MouseTracker
	) override;
};

