#pragma once
#include "UICommon.h"

// RectTransform은 계산만 담당합니다.

class RectTransform
{
public:
    Vector2 m_anchoredPosition{0.0f, 0.0f};
    Vector2 m_size{ 100.0f, 100.0f };
    Vector2 m_pivot{ 0.5f, 0.5f };

    Vector2 m_anchorMin{ 0.5f, 0.5f };
    Vector2 m_anchorMax{ 0.5f, 0.5f };

    UIRect m_worldRect{};
    bool m_dirty = true;

public:
    void MarkDirty(bool v = true) { m_dirty = v; }
    bool IsDirty() const { return m_dirty; }

    void SetSize(float w, float h);
    void SetPivot(float x, float y);
    void SetAnchoredPosition(float x, float y);

    Vector2 GetAnchoredPosition() { return m_anchoredPosition; }  // 로컬
    Vector2 GetWorldTopLeft() const { return m_worldRect.Pos(); } // 월드
    Vector2& AnchoredPositionRef() { return m_anchoredPosition; }

    // 부모의 최종 rect 계산
    void Recalculate(const UIRect& parentRect);

    //Vector3 m_rotation{ 0, 0, 0 };
    //float m_rotationZ = 0.0f;
    Vector3 m_scale{ 1.0f, 1.0f, 1.0f };
};