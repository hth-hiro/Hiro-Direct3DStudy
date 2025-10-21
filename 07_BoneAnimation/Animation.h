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
	void Evaluate(float time, Vector3 position, Quaternion rotation, Vector3 scaling)
	{
		// 현재 시간 기준으로 position, rotation, scaling 보간
	}
};

class Animation
{
public:
	vector<BoneAnimation> BoneAnimations;
	float Duration;
	string Name;
};

