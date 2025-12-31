#include "pch.h"
#include "UIBase.h"
#include "Canvas.h"

static UIRect ToUIRect(const RECT& r)
{
    return UIRect{
        (float)r.left,
        (float)r.top,
        (float)(r.right - r.left),
        (float)(r.bottom - r.top)
    };
}

static RECT ToRect(const UIRect& r)
{
    RECT out;
    out.left = (LONG)r.x;
    out.top = (LONG)r.y;
    out.right = (LONG)(r.x + r.w);
    out.bottom = (LONG)(r.y + r.h);

    return out;
}

void UIBase::Update(float dt)
{
    if (!m_active) return;

    UIRect parentRect{};
    if (m_parent)
    {
        parentRect = ToUIRect(m_parent->GetWorldBounds());
    }
    else
    {
        parentRect = ToUIRect(m_bounds);
    }

    // 부모의 Rect변화 트리거
    bool parentChanged = false;

    if (m_rect.m_dirty)
    {
        m_rect.Recalculate(parentRect);
        m_bounds = ToRect(m_rect.m_worldRect);
        parentChanged = true;
    }

    for (auto& c : m_children)
    {
        if (!c || !c->IsActive()) continue;

        if (parentChanged)
            c->Rect().MarkDirty();

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
    m_rect.SetAnchoredPosition((float)x, (float)y);

    //int w = m_bounds.right - m_bounds.left;
    //int h = m_bounds.bottom - m_bounds.top;

    //m_bounds.left = x;
    //m_bounds.right = x + w;
    //m_bounds.top = y;
    //m_bounds.bottom = y + h;
}

void UIBase::SetPosition(const POINT& p)
{
    m_rect.SetAnchoredPosition(p.x, p.y);
}

POINT UIBase::GetPosition() const
{
    //return POINT{ m_bounds.left, m_bounds.top };
    return POINT{ (LONG)m_rect.m_anchoredPosition.x, (LONG)m_rect.m_anchoredPosition.y };
}

void UIBase::SetSize(int w, int h)
{
    w = std::max(w, 0);
    h = std::max(h, 0);

    m_rect.SetSize((float)w, (float)h);

    //m_bounds.right = m_bounds.left + w;
    //m_bounds.bottom = m_bounds.top + h;
}

void UIBase::SetBounds(const RECT& r)
{
    int x = r.left;
    int y = r.top;

    int w = r.right - r.left;
    int h = r.bottom - r.top;

    SetPosition(x, y);
    SetSize(w, h);
}

Canvas* UIBase::GetCanvas() const
{
    const UIBase* p = this;
    while (p)
    {
        if (auto c = dynamic_cast<const Canvas*>(p))
            return const_cast<Canvas*>(c);

        p = p->GetParent();
    }
    return nullptr;
}

SIZE UIBase::GetSize() const
{
    //return SIZE{
    //    m_bounds.right - m_bounds.left,
    //    m_bounds.bottom - m_bounds.top
    //};

    return SIZE{ (LONG)m_rect.m_size.x, (LONG)m_rect.m_size.y };
}

RECT UIBase::GetWorldBounds() const
{
    //RECT wr = m_bounds;
    //const UIBase* p = m_parent;
    //while (p)
    //{
    //    int px = p->m_bounds.left;
    //    int py = p->m_bounds.top;

    //    OffsetRect(&wr, px, py);
    //    p = p->m_parent;
    //}
    //
    //// 최종 부모(월드)의 바운드
    //return wr;

    return m_bounds;
}