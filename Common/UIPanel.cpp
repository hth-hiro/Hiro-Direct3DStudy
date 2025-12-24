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
        //UIRenderer::Get().DrawFill
    }
}
