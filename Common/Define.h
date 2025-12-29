#pragma once

enum class RunMode
{
    DebugUI,    // 에디터 모드 F1
    Running,    // 게임 실행 F6
    Paused,     // 일시정지 F6
};

inline RunMode g_RunMode = RunMode::DebugUI;

inline bool IsRunning() { return g_RunMode == RunMode::Running; }
inline bool IsPaused() { return g_RunMode == RunMode::Paused; }
inline bool IsDebugUI() { return g_RunMode == RunMode::DebugUI; }