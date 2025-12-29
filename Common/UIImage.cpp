#include "pch.h"
#include "UIImage.h"
#include "UIRenderer.h"

UIImage::UIImage()
{
}

UIImage::~UIImage()
{
    Shutdown();
}

bool UIImage::Initialize()
{
    return true;
}

void UIImage::Shutdown()
{
    m_bitmap.Reset();
}

void UIImage::Render()
{
    if (!IsActive() || !IsVisible()) return;

    if (!m_bitmap) return;

    RECT r = GetWorldBounds();
    if (r.left == r.right || r.top == r.bottom) return;

    UIRenderer::Get().DrawImage(m_bitmap.Get(), r);
}

void UIImage::OnMouseDown(const POINT& p)
{
    if (!IsDebugUI()) return;

    m_dragging = true;

    POINT pos = GetPosition();
    m_dragOffset = { p.x - pos.x, p.y - pos.y };
}

void UIImage::OnMouseMove(const POINT& p)
{
    if (!m_dragging) return;

    SetPosition(p.x - m_dragOffset.x, p.y - m_dragOffset.y);
}

void UIImage::OnMouseUp(const POINT& p)
{
    m_dragging = false;
}

bool UIImage::SetImage(const std::wstring& path)
{
    auto bmp = UIRenderer::Get().LoadBitmapFromFile(path);

    if (!bmp) return false;

    m_bitmap = bmp;
    return true;
}
