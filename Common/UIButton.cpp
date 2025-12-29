#include "pch.h"
#include "UIButton.h"

bool UIButton::Initialize()
{
    m_background = AddChild<UIImage>();
    m_background->SetZOrder(10);

    m_text = AddChild<UIText>();
    m_text->SetZOrder(11);
    return true;
}

void UIButton::SetOnClick(ClickCallback cb)
{
    m_onClick = std::move(cb);
}

void UIButton::Render()
{
    if (!IsVisible() || !IsActive()) return;

    //UpdateVisual();

    RenderChidren();
}

void UIButton::SetImage(const std::wstring& path, State state)
{
    auto bmp = UIRenderer::Get().LoadBitmapFromFile(path);
    if (!bmp) return;

    m_stateImages[state] = bmp;

    if (m_state == state && m_background)
        m_background->SetBitmap(bmp.Get());
}

void UIButton::SetState(State s)
{
    UIBase::SetState(s);
    UpdateVisual();
}

void UIButton::OnMouseEnter()
{
    if (!IsEnabled()) return;
    SetState(State::Hovered);
}

void UIButton::OnMouseLeave()
{

}

void UIButton::OnMouseMove(const POINT&)
{

}

void UIButton::OnMouseDown(const POINT&)
{
    if (!IsEnabled()) return;
    SetState(State::Pressed);
}

void UIButton::OnMouseUp(const POINT&)
{
    if (!IsEnabled()) return;
    SetState(State::Hovered);
}

void UIButton::OnClick()
{
    if (m_onClick)
        m_onClick();
}

// 상태가 전환될때 이미지를 교체
void UIButton::UpdateVisual()
{
    if (!m_background) return;

    auto it = m_stateImages.find(m_state);
    if (it != m_stateImages.end())
    {
        m_background->SetBitmap(it->second.Get());
    }
}