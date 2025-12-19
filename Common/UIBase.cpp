#include "pch.h"
#include "UIBase.h"

void UIBase::Update(float dt)
{
    for (auto& c : m_children)
    {
        if (c && c->IsActive())
            c->Update(dt);
    }
}

void UIBase::Shutdown()
{
    for (auto& c : m_children)
    {
        if (c) c->Shutdown();
    }
    m_children.clear();
}

bool UIBase::HitTest(const POINT& mousePos) const
{
    if (!m_active || !m_visible || !m_enabled)
        return false;

    RECT wr = GetWorldBounds();
    return PtInRect(&wr, mousePos) == TRUE;
}

void UIBase::SetPosition(int x, int y)
{
}

POINT UIBase::GetPosition() const
{
    return POINT();
}

void UIBase::SetSize(int w, int h)
{
}

SIZE UIBase::GetSize() const
{
    return SIZE();
}

RECT UIBase::GetWorldBounds() const
{
    return RECT();
}

void UIBase::AddChild(std::unique_ptr<UIBase> child)
{
}
