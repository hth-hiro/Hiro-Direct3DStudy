#include "pch.h"
#include "UIImage.h"
#include "UIRenderer.h"

UIImage::UIImage()
{
}

bool UIImage::Initialize()
{
    return true;
}

void UIImage::Shutdown()
{
    if (m_isShutdown) return;
    m_isShutdown = true;

    //m_bitmap.Reset();
}

void UIImage::Render()
{
    if (!IsActive() || !IsVisible()) return;

    //if (!m_bitmap) return;

    RECT r = GetWorldBounds();
    if (r.left == r.right || r.top == r.bottom) return;

    auto bmp = UIRenderer::Get().GetBitmap(m_path);

    if (!bmp) UIRenderer::Get().FillRect(r, D2D1::ColorF(D2D1::ColorF::White));
    else UIRenderer::Get().DrawImage(bmp.Get(), r);

    if(!m_children.empty())
        RenderChildren();
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
    if (m_path == path) return false;

    m_path = path;

    //auto bmp = UIRenderer::Get().LoadBitmapFromFile(path);

    //if (!bmp) return false;

    //m_bitmap = bmp;
    return true;
}
