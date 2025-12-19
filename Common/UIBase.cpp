#include "pch.h"
#include "UIBase.h"

bool UIBase::HitTest(const POINT& mousePos)
{
    return false;
}

void UIBase::SetActive(bool active)
{
}

bool UIBase::IsActive() const
{
    return false;
}

void UIBase::AddChild(std::unique_ptr<UIBase> child)
{
}
