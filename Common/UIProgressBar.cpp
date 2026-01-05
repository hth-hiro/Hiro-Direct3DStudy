#include "pch.h"
#include "UIProgressBar.h"
#include "UIRenderer.h"

bool UIProgressBar::Initialize()
{
    m_background = AddChild<UIImage>();
    m_backgroundPath = L"";
    m_background->SetImage(m_backgroundPath);
    m_background->SetTintColor({ 178, 178, 178, 255 });
    m_background->Rect().SetPivot(0.0f, 0.5f);
    m_background->Rect().SetAnchoredPosition(0, 0);
    m_background->SetEnabled(false);

    m_fill = AddChild<UIImage>();
    m_fill->SetImage(m_fillPath);
    if (m_fillPath.empty())
        m_fill->SetTintColor(m_color);
    m_fill->Rect().SetPivot(0.0f, 0.5f);
    m_fill->Rect().SetAnchoredPosition(0, 0);
    m_fill->SetEnabled(false);

    SetSize(200, 20);
    return true;
}

void UIProgressBar::Update(float dt)
{
    UIBase::Update(dt);

    float w = Rect().m_size.x;
    float h = Rect().m_size.y;

    m_background->Rect().SetSize(w, h);

    m_fill->Rect().SetSize(w, h);

    m_background->SetImage(m_backgroundPath);
    m_fill->SetImage(m_fillPath);

    SetProgress(m_progress);
}

void UIProgressBar::Render()
{
    if (!IsActive() || !IsVisible()) return;

    if (m_background) m_background->Render();

    if (m_fill)
    {
        RECT fillRect = m_fill->GetWorldBounds();
        float width = (float)(fillRect.right - fillRect.left);

        RECT clipRect = fillRect;
        clipRect.right = (long)(fillRect.left + (width * m_progress));
        UIRenderer::Get().PushClip(clipRect);
        m_fill->Render();
        UIRenderer::Get().PopClip();
    }
    //RenderChildren();
}

void UIProgressBar::SetProgress(float ratio)
{
    if (m_progress == ratio) return;

    m_progress = std::clamp(ratio, 0.0f, 1.0f);
}

void UIProgressBar::SetImage(const std::wstring& path, ImageType type)
{
    if (type == ImageType::Frame)
    {
        m_backgroundPath = path;
    }
    else
    {
        m_fillPath = path;
    }
}

void UIProgressBar::SetFillColor(const Vector4& color)
{
    m_color = color;
}
