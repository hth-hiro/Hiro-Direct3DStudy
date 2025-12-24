#pragma once
#include <iostream> 
#include "Helper.h"

#include <wrl/client.h>
#include <d3d11.h>
#include <dxgi1_2.h>
#include <d2d1_1.h>
#include <dwrite.h>
#include <string>

#include <DirectXMath.h>
#include <map>

#include <filesystem>

using Microsoft::WRL::ComPtr;
using namespace DirectX;

// UIBase는 UI의 부모입니다.
// 모든 UI들을 UIBase가 보관하고, UIManager가 UIBase를 관리합니다.
class UIBase
{
public:
    enum class State
    {
        Normal, Hovered, Pressed, Disabled
    };

public:
    virtual ~UIBase() = default;

    virtual bool Initialize() { return true; }
    virtual void Update(float dt);
    virtual void Render() = 0;
    virtual void Shutdown();

    virtual bool HitTest(const POINT& mousePos) const;

    void SetActive(bool active) { m_active = active; }
    bool IsActive() const { return m_active; }

    void SetVisible(bool visible) { m_visible = visible; }
    bool IsVisible() const { return m_visible; }

    void SetEnabled(bool enabled)
    {
        m_enabled = enabled;
        m_state = m_enabled ? State::Normal : State::Disabled;
    }
    bool IsEnabled() const { return m_enabled; }

    State GetState() const { return m_state; }
    void SetState(State s) { m_state = s; }

    int GetZOrder() const { return m_zOrder; }
    void SetZOrder(int z) { m_zOrder = z; }

    void SetBounds(const RECT& r) { m_bounds = r; }
    const RECT& GetBounds() const { return m_bounds; }

    void SetPosition(int x, int y);
    POINT GetPosition() const;

    void SetSize(int w, int h);
    SIZE GetSize() const;

    RECT GetWorldBounds() const;

    // Tree구조를 만든다.
    void AddChild(std::unique_ptr<UIBase> child);
    UIBase* GetParent() const { return m_parent; }

    // 버튼과 상호작용하는 함수, UIManager가 호출
    virtual void OnMouseEnter() {}
    virtual void OnMouseLeave() {}
    virtual void OnMouseMove(const POINT&) {}
    virtual void OnMouseDown(const POINT&) {}
    virtual void OnMouseUp(const POINT&) {}
    virtual void OnClick() {}

protected:
    RECT m_bounds = {0, 0, 0, 0};

    UIBase* m_parent = nullptr;
    std::vector<std::unique_ptr<UIBase>> m_children;
    
    int m_zOrder = 0;

    bool m_active = true;
    bool m_visible = true;
    bool m_enabled = true;

    State m_state = State::Normal;
};

