#include "pch.h"
#include "UIToggle.h"

bool UIToggle::Initialize()
{
    m_background = AddChild<UIImage>();
    m_background->SetImage(m_backgroundPath);   // 기본 경로
    m_background->Rect().SetPivot(0.0f, 0.0f);
    m_background->Rect().SetAnchoredPosition(0, 0);
    m_background->SetEnabled(false);

    m_checkmark = AddChild<UIImage>();
    m_checkmark->SetImage(m_checkmarkPath);     // 기본 경로
    m_checkmark->Rect().SetPivot(0.0, 0.0);
    m_checkmark->Rect().SetAnchoredPosition(0, 0);
    m_checkmark->SetEnabled(false);

    // Visible로 체크 상태 확인
    m_checkmark->SetVisible(m_isOn);

    SetSize(30, 30);
    return true;
}

void UIToggle::Update(float dt)
{
    UIBase::Update(dt);

    float parentW = Rect().m_size.x;
    float parentH = Rect().m_size.y;

    if (m_background)
    {
        m_background->Rect().SetSize(parentW, parentH);
    }

    if (m_checkmark)
    {
        m_checkmark->Rect().SetSize(parentW, parentH);
    }

    m_background->SetImage(m_backgroundPath);
    m_checkmark->SetImage(m_checkmarkPath);
}

void UIToggle::Render()
{
    if (!IsActive() || !IsVisible()) return;
    RenderChildren();
}

void UIToggle::SetIsOn(bool isOn, bool notify)
{
    if (m_isOn == isOn) return;

    m_isOn = isOn;

    // 체크마크 가시성 조절
    if (m_checkmark)
    {
        m_checkmark->SetVisible(m_isOn);
    }

    if (notify && m_onValueChanged)
        m_onValueChanged(m_isOn);
}

void UIToggle::SetImage(const std::wstring& path, ImageType type)
{
    if (type == ImageType::Frame)
    {
        m_backgroundPath = path;
    }
    else
    {
        m_checkmarkPath = path;
    }
}

void UIToggle::SetOnCheckedChanged(OnToggleChanged cb)
{
    m_onValueChanged = std::move(cb);
}

void UIToggle::OnMouseDown(const POINT& pt)
{
    SetIsOn(!m_isOn);

    // 디버깅 출력
    std::wstring msg = L"Toggle 상태 변경: " + std::wstring(m_isOn ? L"ON" : L"OFF") + L"\n";
    OutputDebugStringW(msg.c_str());
}
