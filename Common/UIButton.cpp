#include "pch.h"
#include "UIButton.h"

void UIButton::SetOnClick(ClickCallback cb)
{
}

void UIButton::Render()
{
    RECT r = GetWorldBounds();

    // TODO: 함수 구현
    switch (GetState())
    {
    case State::Normal:
        // 노멀상태의 이미지
        break;

    case State::Hovered:
        break;

    case State::Pressed:
        break;
    }
}

void UIButton::OnMouseEnter()
{
    if (!IsEnabled) return;
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
    if (!IsEnabled) return;
    SetState(State::Pressed);
}

void UIButton::OnMouseUp(const POINT&)
{
    if (m_onClick)
        m_onClick();
}

void UIButton::OnClick()
{
}
