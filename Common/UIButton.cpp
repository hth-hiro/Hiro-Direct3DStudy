#include "pch.h"
#include "UIButton.h"

bool UIButton::Initialize()
{
    m_background = AddChild<UIImage>();
    m_background->Rect().SetPivot(0, 0);
    m_background->Rect().SetAnchoredPosition(0, 0);
    m_background->Rect().SetSize(Rect().m_size.x, Rect().m_size.y);
    m_background->SetEnabled(false);
    
    m_text = AddChild<UIText>();
    m_text->SetText(L"Button");
    m_text->Rect().SetPivot(0.5, 0.5);
    m_text->Rect().SetAnchoredPosition(Rect().m_size.x * 0.5f, Rect().m_size.y * 0.5f);
    m_text->Rect().SetSize(Rect().m_size.x, Rect().m_size.y);
    m_text->SetEnabled(false);

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
    //RECT r = GetWorldBounds();
    //if (r.left == r.right || r.top == r.bottom) return;

    //UIRenderer::Get().DrawImage(m_stateImages[m_state].Get(), r);

    RenderChildren();
}

void UIButton::SetSize(int w, int h)
{
    UIBase::SetSize(w, h);

    if (m_background)
        m_background->Rect().SetSize((float)w, (float)h);

    if (m_text)
        m_text->Rect().SetSize((float)w, (float)h);
}

void UIButton::SetText(const std::wstring& text)
{
    m_text->Rect().SetPivot(0.5, 0.5);
    m_text->Rect().SetAnchoredPosition(Rect().m_size.x * 0.5f, Rect().m_size.y * 0.5f);
    m_text->Rect().SetSize(Rect().m_size.x, Rect().m_size.y);

    m_text->SetText(text);
}

void UIButton::SetImage(const std::wstring& path, State state)
{
    //auto bmp = UIRenderer::Get().LoadBitmapFromFile(path);
    //if (!bmp) return;

    m_statePaths[state] = path;
    if (m_state == state) UpdateVisual();

    //if (m_state == state && m_background)
    //    m_background->SetBitmap(bmp.Get());
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
    if (!IsEnabled()) return;
    SetState(State::Normal);
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
    auto it = m_statePaths.find(m_state);
    if (it != m_statePaths.end())
    {
        m_background->SetImage(it->second);
    }
}