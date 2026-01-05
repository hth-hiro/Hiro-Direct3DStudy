#pragma once
#include <vector>
#include <memory>
#include <cstdint>
#include <string>
#include <algorithm>
#include <functional>

#include <d3d11.h>
#include <directxtk/SimpleMath.h>

using namespace DirectX::SimpleMath;
using namespace DirectX;

template <typename T>
T Max(const T& a, const T& b)
{
    return a > b ? a : b;
}

enum class AnchorPreset
{
    TopLeft = 0, TopCenter, TopRight,
    MiddleLeft, MiddleCenter, MiddleRight,
    BottomLeft, BottomCenter, BottomRight,

    TL = 0, TC = 1, TR = 2,
    ML = 3, MC = 4, MR = 5,
    BL = 6, BC = 7, BR = 8,
};

struct UIColor { uint8_t r = 255, g = 255, b = 255, a = 255; };

struct UIRect
{
    float x = 0.0f;
    float y = 0.0f;
    float w = 0.0f;
    float h = 0.0f;

    Vector2 Pos() const { return { x, y }; }
    Vector2 Size() const { return { w, h }; }

    // 점이 UIRect에 들어와 있는지 검사
    bool Contains(const Vector2& p)const
    {
        return p.x >= Left() && p.x <= Right() && p.y >= Top() && p.y <= Bottom();
    }

private:
    float Left()   const { return x; }
    float Right()  const { return x + w; }
    float Top()    const { return y; }
    float Bottom() const { return y + h; }
};

// Dirty Flag - 성능 향상을 위함
// 다음 구조체는 여러 플래그를 관리하기 용이하게 하기 위해 | 연산자 사용
// operator 구현
enum class UIDirtyFlags : uint32_t
{
    None = 0,
    Layout = 1 << 0,             // RectTransform 배치
    Geometry = 1 << 1,           // 버텍스/인덱스 재생성
    Material = 1 << 2,           // 텍스처/머티리얼 변경
    Hirearchy = 1 << 3,          // 순서/부모자식 변경
};

inline UIDirtyFlags operator|(UIDirtyFlags a, UIDirtyFlags b)
{
    return (UIDirtyFlags)((uint32_t)a |(uint32_t)b);
}

inline UIDirtyFlags& operator|=(UIDirtyFlags& a, UIDirtyFlags& b)
{
    a = a | b;
    return a;
}

inline bool HasFlag(UIDirtyFlags a, UIDirtyFlags b)
{
    return (((uint32_t)a & (uint32_t)b) != 0);
}

struct UIEvent
{
    enum class Type {PointerMove, PointerDown, PointerUp, PointerClick } type;

    Vector2 screenPos;
    int button = 0;
};