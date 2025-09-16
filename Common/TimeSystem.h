#pragma once
#include <Windows.h>

// Singleton

class TimeSystem
{
public:
	TimeSystem();

	void Initialize();
	void Update();

	float GetDeltaTime() const;
	float GetTotalTime() const;
	float GetPlayTime() const { return playTime; }

	void SetTotalTime(float time);
	void SetPlayTime(float time);

	void Pause() { m_isPaused = true; }
	void Resume() { m_isPaused = false; }
	bool isPaused() const { return m_isPaused; }

	float GetTimeScale() const { return m_timeScale; }
	void SetTimeScale(float scale) { m_timeScale = scale; }

	static TimeSystem* m_Instance;

private:
	LARGE_INTEGER frequency;
	LARGE_INTEGER prevCounter;
	LARGE_INTEGER currentCounter;
	LARGE_INTEGER initCounter;

	float deltaTime = 0.0f;
	float totalTime = 0.0f;
	float playTime = 0.0f;

	float m_timeScale = 1.0f;
	bool m_isPaused = false;
};

