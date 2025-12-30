#include "pch.h"
#include "UIPanel.h"
#include "UIRenderer.h"

void UIPanel::Render()
{
    if (!IsActive() || !IsVisible())
        return;

    if (m_drawBackground)
    {
        RECT wr = GetWorldBounds();

        UIRenderer::Get().FillRect(wr, m_bgColor);
    }

    RenderChildren();
}

void UIPanel::SetRect(int x, int y, int width, int height)
{
    Rect().SetPivot(0.0f, 0.0f);

    Rect().SetAnchoredPosition((float)x, (float)y);
    Rect().SetSize((float)width, (float)height);

    m_rect.m_dirty = true;

    //SetBounds({ x, y, width, height });
}