#pragma once
#include <directxtk/SimpleMath.h>
#include <vector>
#include <string>
#include <d3d11_1.h>
#include <DirectXMath.h>

using namespace DirectX;
using namespace DirectX::SimpleMath;
using namespace std;

struct AnimationKey
{
	float Time;
	Vector3 Position;
	Quaternion Rotation;
	Vector3 Scaling;
};

struct BoneAnimation
{
	vector<AnimationKey> AnimationKeys;
	void Evaluate(float time, Vector3& position, Quaternion& rotation, Vector3& scaling)
	{
		// 현재 시간 기준으로 position, rotation, scaling 보간
		if (AnimationKeys.empty())
		{
			position = Vector3::Zero;
			rotation = Quaternion::Identity;
			scaling = Vector3::One;
			return;
		}

		if (AnimationKeys.size() == 1)
		{
			position = AnimationKeys[0].Position;
			rotation = AnimationKeys[0].Rotation;
			scaling = AnimationKeys[0].Scaling;
			return;
		}

		UINT keyIndex = 0;
		for (; keyIndex < AnimationKeys.size() - 1; keyIndex++)
		{
			if (time < AnimationKeys[keyIndex + 1].Time)
				break;
		}

		const AnimationKey& key1 = AnimationKeys[keyIndex];
		const AnimationKey& key2 = AnimationKeys[keyIndex + 1];

		float delta = key2.Time - key1.Time;
		float t = 0.0f;
		if (delta > 0.0f)
			t = (time - key1.Time) / delta;

		position = Vector3::Lerp(key1.Position, key2.Position, t);
		rotation = Quaternion::Slerp(key1.Rotation, key2.Rotation, t);
		scaling = Vector3::Lerp(key1.Scaling, key2.Scaling, t);
	}
};

class Animation
{
public:
	vector<BoneAnimation> BoneAnimations;
	float Duration;
	string Name;
};

