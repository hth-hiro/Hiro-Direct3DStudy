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

bool UIImage::SetImage(const std::wstring& path)
{
    auto bmp = UIRenderer::Get().LoadBitmapFromFile(path);

    if (!bmp) return false;

    m_bitmap = bmp;
    return true;
}
