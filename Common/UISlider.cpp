#include "pch.h"
#include "UISlider.h"

bool UISlider::Initialize()
{
    m_track = AddChild<UIImage>();
    m_track->SetImage(m_trackPath);
    m_track->SetTintColor({ 52, 135, 255, 255 });

    m_track->Rect().SetPivot(0.5, 0.5);
    m_track->Rect().SetAnchoredPosition(Rect().m_size.x * 0.5f, Rect().m_size.y * 0.5f);

    // 방향에 따른 사이즈 설정
    if (m_orientation == SliderOrientation::Horizontal)
        m_track->Rect().SetSize(Rect().m_size.x, 10);
    else
        m_track->Rect().SetSize(10, Rect().m_size.y);
    m_track->SetEnabled(false);

    m_handle = AddChild<UIImage>();
    m_handle->SetImage(m_handlePath);
    m_handle->Rect().SetPivot(0.5f, 0.5f);
    m_handle->Rect().SetSize(20, 20);
    m_handle->Rect().SetAnchoredPosition(Rect().m_size.x * 0.5f, Rect().m_size.y * 0.5f);
    m_handle->SetEnabled(false);

    UpdateThumbByValue();
    return true;
}

void UISlider::Render()
{
    if (!IsVisible() || !IsActive()) return;

    RenderChildren();
}

void UISlider::Update(float dt)
{
    UIBase::Update(dt);

    if (m_track && m_handle)
    {
        float w = Rect().m_size.x;
        float h = Rect().m_size.y;

        //m_track->Rect().SetAnchoredPosition(0, h * 0.5f);
        //m_track->Rect().SetSize(w, 10);

        m_track->Rect().SetAnchoredPosition(w * 0.5f, h * 0.5f);

        if (m_orientation == SliderOrientation::Horizontal)
            m_track->Rect().SetSize(w, 10);
        else
            m_track->Rect().SetSize(10, h);

        // 핸들의 Y축 위치도 중앙으로
        float thumbY = h * 0.5f;

        // 현재 value에 따른 X 위치 재계산
        UpdateThumbByValue();
    }

    m_handle->SetImage(m_handlePath);
    m_track->SetImage(m_trackPath);
}

void UISlider::SetImage(const std::wstring& path, ImageType type)
{
    if (type == ImageType::Track)
    {
        m_trackPath = path;
    }
    else
    {
        m_handlePath = path;
    }
}

void UISlider::SetRange(float min, float max)
{
    m_min = min;
    m_max = max;
}

void UISlider::SetValue(float v, bool notify)
{
    v = std::clamp(v, m_min, m_max);
    if (v == m_value) return;

    m_value = v;
    UpdateThumbByValue();

    if (notify && m_onValueChanged)
        m_onValueChanged(m_value);
}

void UISlider::SetOnValueChanged(ValueChangedCallback cb)
{
    m_onValueChanged = std::move(cb);
}

void UISlider::OnMouseDown(const POINT& pt)
{
    m_dragging = true;
    UpdateValueByMouse(pt);
}

void UISlider::OnMouseMove(const POINT& pt)
{
    if (!m_dragging) return;
    UpdateValueByMouse(pt);
}

void UISlider::OnMouseUp(const POINT& pt)
{
    m_dragging = false;
}

void UISlider::UpdateThumbByValue()
{
    if (!m_track || !m_handle) return;

    // 0.0 ~ 1.0 사이의 비율 계산
    float t = (m_max != m_min) ? (m_value - m_min) / (m_max != m_min) : 0.0f;
    t = std::clamp(t, 0.0f, 1.0f);

    // 트랙의 크기를 기준으로 로컬 좌표 계산
    float trackW = m_track->Rect().m_size.x;

    float w = Rect().m_size.x;
    float h = Rect().m_size.y;

    float thumbW = m_handle->Rect().m_size.x;
    float thumbH = m_handle->Rect().m_size.y;

    if (m_orientation == SliderOrientation::Horizontal)
    {
        float x_pos = (thumbW * 0.5f) + (w - thumbW) * t;
        m_handle->Rect().SetAnchoredPosition(x_pos, h * 0.5f);
    }
    else
    {
        float y_pos = (h - thumbH * 0.5f) - (h - thumbH) * t;
        m_handle->Rect().SetAnchoredPosition(w * 0.5f, y_pos);
    }
}

void UISlider::UpdateValueByMouse(const POINT& pt)
{
    if (!m_track || !m_handle) return;

    RECT trackRect = m_track->GetWorldBounds();
    float t = 0.0f;

    if (m_orientation == SliderOrientation::Horizontal)
    {
        float trackWorldX = (float)trackRect.left;
        float trackWorldW = (float)(trackRect.right - trackRect.left);

        // 유효 가동 범위 계산
        float thumbW = m_handle->Rect().m_size.x;
        float minWorldX = trackWorldX + (thumbW * 0.5f);
        float maxWorldX = trackWorldX + trackWorldW - (thumbW * 0.5f);

        // 마우스 좌표 정규화
        if (maxWorldX > minWorldX)
        {
            t = ((float)pt.x - minWorldX) / (maxWorldX - minWorldX);
        }
        else
        {
            t = 0.5f;
        }
    }
    else
    {
        float trackWorldY = (float)trackRect.top;
        float trackWorldH = (float)(trackRect.bottom - trackRect.top);

        float thumbH = m_handle->Rect().m_size.y;
        float minWorldY = trackWorldY + trackWorldH - (thumbH * 0.5f);
        float maxWorldY = trackWorldY + (thumbH * 0.5f);

        if (minWorldY != maxWorldY)
        {
            t = ((float)pt.y - minWorldY) / (maxWorldY - minWorldY);
        }
        else
        {
            t = 0.5f;
        }
    }

    t = std::clamp(t, 0.0f, 1.0f);
    float newValue = m_min + (m_max - m_min) * t;
    SetValue(newValue);
}
