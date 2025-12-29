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
#include <concepts>

using Microsoft::WRL::ComPtr;
using namespace DirectX;

class UIBase;

template <typename T>
concept Bases = std::derived_from<T, UIBase>;

// UIBase는 UI의 부모입니다.
// 모든 UI들을 UIBase가 보관하고, UISystem이 UIBase를 관리합니다.
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

    // Active - UI Update 여부
    void SetActive(bool active) { m_active = active; }
    bool IsActive() const { return m_active; }

    // Visible - Render 대상 여부 (투명이지만 입력을 받는 경우, 미리 생성하고 나중에 보여질때)
    void SetVisible(bool visible) { m_visible = visible; }
    bool IsVisible() const { return m_visible; }

    // Enable - 조작 가능 여부 (Render/Update는 되지만 입력을 안 받는 UI)
    void SetEnabled(bool enabled)
    {
        m_enabled = enabled;
        m_state = m_enabled ? State::Normal : State::Disabled;
    }
    bool IsEnabled() const { return m_enabled; }

    State GetState() const { return m_state; }
    virtual void SetState(State s) { m_state = s; }

    // 표시할 레이어를 구분합니다.
    // 레이어 관련 내용은 enum class로 정리하면 좋을 것 같습니다.
    int GetZOrder() const { return m_zOrder; }
    void SetZOrder(int z) { m_zOrder = z; }

    void SetBounds(const RECT& r) { m_bounds = r; }
    const RECT& GetBounds() const { return m_bounds; }

    void SetPosition(int x, int y);
    POINT GetPosition() const;

    void SetSize(int w, int h);
    SIZE GetSize() const;

    RECT GetWorldBounds() const;

    // Tree구조를 만든다. - 템플릿으로 자식객체 추가
    template <typename T, typename...Args>
    T* AddChild(Args&&... args);
    UIBase* GetParent() const { return m_parent; }

    template <class T>
    T* FindFirst()
    {
        for (auto& c : m_children)
        {
            if (auto* p = dynamic_cast<T*>(c.get())) return p;
            if (auto* r = c->FindFirst<T>()) return r;
        }

        return nullptr;
    }

    UIBase* Pick(const POINT& p)
    {
        if (!IsActive() || !IsVisible() || !IsEnabled()) return nullptr;

        for (auto it = m_children.rbegin(); it != m_children.rend(); ++it)
        {
            if (UIBase* hit = (*it)->Pick(p)) return hit;
        }

        if (HitTest(p)) return this;

        return nullptr;
    }

    template<Bases T>
    T* PickAs(const POINT& p)
    {
        if (UIBase* hit = Pick(p))
            return dynamic_cast<T*>(hit);
        return nullptr;
    }

    // UIButton과 상호작용하는 함수, UISystem이 호출
    virtual void OnMouseEnter() {}
    virtual void OnMouseLeave() {}
    virtual void OnMouseMove(const POINT&) {}
    virtual void OnMouseDown(const POINT&) {}
    virtual void OnMouseUp(const POINT&) {}
    virtual void OnClick() {}

    // UIPanel과 상호작용하는 함수
    virtual bool BlocksInput() const { return false; }
    void RenderChidren()
    {
        for (auto& c : m_children)
        {
            if (!c) continue;
            if (!c->IsActive() || !c->IsVisible())continue;
            c->Render();
        }
    }

public:
    // 자식 노드 수를 가져옴
    int GetChildCount() const { return m_children.size(); }
    UIBase* GetChild(int i) const { return m_children[i].get(); }

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

template<typename T, typename ...Args>
inline T* UIBase::AddChild(Args && ...args)
{
    static_assert(std::is_base_of_v<UIBase, T>, "T must derive from, UIBase");

    auto child = std::make_unique<T>(std::forward<Args>(args)...);
    child->m_parent = this;

    child->Initialize();

    T* raw = child.get();
    m_children.emplace_back(std::move(child));

    return raw;
}