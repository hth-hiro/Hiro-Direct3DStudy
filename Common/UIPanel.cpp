#include "pch.h"
#include "UIPanel.h"
#include "UIRenderer.h"

void UIPanel::Render()
{
    if (!IsVisible())
        return;

    if (m_drawBackground)
    {
        RECT wr = GetWorldBounds();

        UIRenderer::Get().FillRect(wr, m_bgColor);
    }

    RenderChidren();
}

void UIPanel::SetRect(int x, int y, int width, int height)
{
    SetBounds({ x, y, width, height });
}