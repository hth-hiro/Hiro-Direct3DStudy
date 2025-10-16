#pragma once

#include <vector>
#include <d3d11_1.h>
#include <DirectXMath.h>

using namespace DirectX;

class Transform
{
	XMMATRIX cachedMatrix = XMMatrixIdentity();
	bool dirty = true;

public:
	Transform() = default;
	Transform(const XMFLOAT3& pos) : position(pos) {}

	XMFLOAT3 position = { 0,0,0 };
	XMFLOAT3 rotation = { 0,0,0 };
	XMFLOAT3 scale = { 1,1,1 };

	void AddPosition(const XMFLOAT3& delta) {
		position.x += delta.x;
		position.y += delta.y;
		position.z += delta.z;
		dirty = true;
	}

	void AddRotation(const XMFLOAT3& delta) {
		rotation.x += delta.x;
		rotation.y += delta.y;
		rotation.z += delta.z;
		dirty = true;
	}

	void AddScale(const XMFLOAT3& factor) {
		scale.x *= factor.x;
		scale.y *= factor.y;
		scale.z *= factor.z;
		dirty = true;
	}

	void SetPosition(const XMFLOAT3& position_) { position = position_; dirty = true; }
	void SetRotation(const XMFLOAT3& rotation_) { rotation = rotation_; dirty = true; }
	void SetScale(const XMFLOAT3& scale_) { scale = scale_; dirty = true; }

	const XMFLOAT3& GetPosition() const { return position; }
	const XMFLOAT3& GetRotation() const { return XMFLOAT3(rotation.y / 180 * XM_PI, rotation.x / 180 * XM_PI, rotation.z / 180 * XM_PI); }
	const XMFLOAT3& GetScale()    const { return scale; }

	XMMATRIX GetMatrix() {
		//if (dirty)
		{
			XMMATRIX S = XMMatrixScaling(scale.x, scale.y, scale.z);
			XMMATRIX R = XMMatrixRotationRollPitchYaw(rotation.x, rotation.y, rotation.z);
			XMMATRIX T = XMMatrixTranslation(position.x, position.y, position.z);

			cachedMatrix = S * R * T;
			dirty = false;
		}

		return cachedMatrix;
	}
};