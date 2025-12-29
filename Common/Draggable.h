#pragma once
#include <d3d11.h>
#include <directxtk/SimpleMath.h>
using namespace DirectX::SimpleMath;
using namespace DirectX;

#if defined(_DEBUG)

template <class T>
concept HsTransform = requires(T t, Vector2 p)
{
    { t.GetPosition() } -> std::same_as<Vector2>;
    t.SetPosition(p);
};

template <HsTransform T>
struct Draggable
{
    bool dragging = false;
    Vector2 offset{};
    
    void Begin(T& w, Vector2 mouse)
    {
        dragging = true;
        offset = mouse - w.GetPosition();
    }

    void Move(T& w, Vector2 mouse)
    {
        if (dragging)
            w.SetPosition(mouse - offset);
    }

    void End() { dragging = false; }
};

#endif