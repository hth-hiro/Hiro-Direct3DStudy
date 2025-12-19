#include "pch.h"
#include "UIBase.h"

bool UIBase::HitTest(const POINT& mousePos)
{
    return false;
}

void UIBase::SetActive(bool active)
{
    m_active = active;
}

bool UIBase::IsActive() const
{
    return m_active;
}

void UIBase::AddChild(std::unique_ptr<UIBase> child)
{
}
