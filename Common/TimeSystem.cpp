#include "pch.h"
#include "TimeSystem.h"

TimeSystem* TimeSystem::m_Instance = nullptr;

TimeSystem::TimeSystem()
{
    if (m_Instance == nullptr)
    {
        m_Instance = this;
    }
}

void TimeSystem::Initialize()
{
    QueryPerformanceFrequency(&frequency);
    QueryPerformanceCounter(&initCounter);
    prevCounter = initCounter;
}

void TimeSystem::Update()
{
    QueryPerformanceCounter(&currentCounter);

    float frameTime = static_cast<float>(currentCounter.QuadPart - prevCounter.QuadPart) / frequency.QuadPart;

    if (!m_isPaused)
    {
        deltaTime = frameTime * m_timeScale;
        playTime += deltaTime;
    }
    else
    {
        deltaTime = 0.0f;
    }

    prevCounter = currentCounter;
}

float TimeSystem::GetDeltaTime() const
{
    return deltaTime;
}

float TimeSystem::GetTotalTime() const
{
    return static_cast<float>(currentCounter.QuadPart - initCounter.QuadPart) / frequency.QuadPart;
}

void TimeSystem::SetTotalTime(float time)
{
    LARGE_INTEGER counter;
    QueryPerformanceCounter(&counter);
    initCounter.QuadPart = counter.QuadPart - static_cast<LONGLONG>(time * frequency.QuadPart);
}

void TimeSystem::SetPlayTime(float time)
{
    playTime = time;
}
