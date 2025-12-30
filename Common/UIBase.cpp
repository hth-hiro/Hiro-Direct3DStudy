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
    int w = m_bounds.right - m_bounds.left;
    int h = m_bounds.bottom - m_bounds.top;

    m_bounds.left = x;
    m_bounds.right = x + w;
    m_bounds.top = y;
    m_bounds.bottom = y + h;
}

POINT UIBase::GetPosition() const
{
    return POINT{ m_bounds.left, m_bounds.top };
}

void UIBase::SetSize(int w, int h)
{
    w = std::max(w, 0);
    h = std::max(h, 0);

    m_bounds.right = m_bounds.left + w;
    m_bounds.bottom = m_bounds.top + h;
}

SIZE UIBase::GetSize() const
{
    return SIZE{
        m_bounds.right - m_bounds.left,
        m_bounds.bottom - m_bounds.top
    };
}

RECT UIBase::GetWorldBounds() const
{
    RECT wr = m_bounds;
    const UIBase* p = m_parent;
    while (p)
    {
        int px = p->m_bounds.left;
        int py = p->m_bounds.top;

        OffsetRect(&wr, px, py);
        p = p->m_parent;
    }
    
    // 최종 부모(월드)의 바운드
    return wr;
}