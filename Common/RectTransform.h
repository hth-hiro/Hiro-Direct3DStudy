#pragma once
#include <algorithm>
#include <directxtk/SimpleMath.h>

using namespace DirectX::SimpleMath;
using namespace DirectX;

template <typename T>
T Max(const T& a, const T& b)
{
    return a > b ? a : b;
}

enum class Pivot
{
    TopLeft = 0, TopCenter, TopRight,
    MiddleLeft, MiddleCenter, MiddleRight,
    BottomLeft, BottomCenter, BottomRight,

    TL = 0, TC = 1, TR = 2,
    ML = 3, MC = 4, MR = 5,
    BL = 6, BC = 7, BR = 8,
};

struct UIRect
{
    float x = 0.0f;
    float y = 0.0f;
    float w = 0.0f;
    float h = 0.0f;

    Vector2 Pos() const { return { x, y }; }
    Vector2 Size() const { return { w, h }; }
};

class RectTransform
{
public:
    Vector2 m_anchoredPosition{0.0f, 0.0f};
    Vector2 m_anchorMin{ 0.5f, 0.5f };
    Vector2 m_anchorMax{ 0.5f, 0.5f };
    Vector2 m_pivot{ 0.5f, 0.5f };

    //Vector3 m_rotation{ 0, 0, 0 };
    float m_rotationZ = 0.0f;
    Vector2 m_size{ 100.0f, 100.0f };
    Vector3 m_scale{ 1.0f, 1.0f, 1.0f };

    bool m_dirty = true;
    
    // 계산 결과를 저장할 구조체
    UIRect m_worldRect{};

public:
    RectTransform() = default;
    ~RectTransform() = default;

    void MarkDirty() { m_dirty = true; }

    void SetSize(float w, float h);
    void SetPivot(float x, float y);
    void SetAnchoredPosition(float x, float y);

    Vector2 GetAnchoredPosition() { return m_anchoredPosition; }  // 로컬
    Vector2 GetWorldTopLeft() const { return m_worldRect.Pos(); } // 월드
    Vector2& AnchoredPositionRef() { return m_anchoredPosition; }

    // 부모의 최종 rect 계산
    void Recalculate(const UIRect& parentRect)
    {
        Vector2 size = m_size;

        const Vector2 parentTopLeft = parentRect.Pos();
        const Vector2 pivotPos = parentTopLeft + m_anchoredPosition;
        const Vector2 topLeft = pivotPos - Vector2(size.x * m_pivot.x, size.y * m_pivot.y);

        m_worldRect.x = topLeft.x;
        m_worldRect.y = topLeft.y;
        m_worldRect.w = size.x;
        m_worldRect.h = size.y;

        m_dirty = false;
    }
};