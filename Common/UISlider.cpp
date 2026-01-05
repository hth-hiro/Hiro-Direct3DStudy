#include "pch.h"
#include "UISlider.h"

bool UISlider::Initialize()
{
    m_track = AddChild<UIImage>();
    m_track->Rect().SetPivot(0.0, 0.5);
    m_track->Rect().SetAnchoredPosition(0, 0);
    m_track->Rect().SetSize(Rect().m_size.x, 10);
    m_track->SetEnabled(false);

    m_handle = AddChild<UIImage>();
    m_handle->Rect().SetPivot(0.5f, 0.5f);
    m_handle->Rect().SetSize(20, 20);
    m_handle->Rect().SetAnchoredPosition(0, Rect().m_size.y * 0.5f);
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
        float currentHeight = Rect().m_size.y;
        float currentWidth = Rect().m_size.x;

        // 트랙의 높이를 슬라이더의 중앙으로 (Pivot 0.5 기준)
        m_track->Rect().SetAnchoredPosition(0, currentHeight * 0.5f);
        m_track->Rect().SetSize(currentWidth, 10);

        // 핸들의 Y축 위치도 중앙으로
        float thumbY = currentHeight * 0.5f;

        // 현재 value에 따른 X 위치 재계산
        UpdateThumbByValue();
    }

    m_handle->SetImage(m_thumbPath);
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
        m_thumbPath = path;
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
    float denom = (m_max - m_min);
    float t = (denom != 0.0f) ? (m_value - m_min) / denom : 0.0f;
    t = std::clamp(t, 0.0f, 1.0f);

    // 트랙의 크기를 기준으로 로컬 좌표 계산
    float trackW = m_track->Rect().m_size.x;
    float thumbW = m_handle->Rect().m_size.x;

    // 핸들이 트랙 밖으로 나가지 않게 하려면 여백을 고려합니다.
    // minX: 트랙 시작점 + 핸들 절반, maxX: 트랙 끝점 - 핸들 절반
    float minX = thumbW * 0.5f;
    float maxX = trackW - thumbW * 0.5f;

    // 트랙이 매우 짧을 경우를 대비
    if (maxX < minX) maxX = minX;

    float x_local = minX + (maxX - minX) * t;

    // 트랙의 로컬 위치를 기준으로 핸들 위치 설정
    // m_track의 pivot이 (0, 0.5)이므로 track의 anchoredPosition.x가 곧 시작점입니다.
    x_local += m_track->Rect().m_anchoredPosition.x;
    float y_local = m_track->Rect().m_anchoredPosition.y;

    m_handle->Rect().SetAnchoredPosition(x_local, y_local);
}

void UISlider::UpdateValueByMouse(const POINT& pt)
{
    if (!m_track || !m_handle) return;

    // 1. 트랙의 월드 바운드를 가져옵니다. (가장 정확한 기준)
    RECT trackRect = m_track->GetWorldBounds();
    float trackWorldX = (float)trackRect.left;
    float trackWorldW = (float)(trackRect.right - trackRect.left);

    // 2. 핸들의 폭을 고려한 유효 가동 범위를 계산합니다.
    float thumbW = m_handle->Rect().m_size.x;
    float minWorldX = trackWorldX + (thumbW * 0.5f);
    float maxWorldX = trackWorldX + trackWorldW - (thumbW * 0.5f);

    // 3. 마우스 좌표(pt.x)를 이 범위 내에서 0.0 ~ 1.0으로 정규화합니다.
    float t = 0.0f;
    if (maxWorldX > minWorldX)
    {
        t = ((float)pt.x - minWorldX) / (maxWorldX - minWorldX);
    }
    else
    {
        // 트랙이 핸들보다 작을 경우 중앙값 처리
        t = 0.5f;
    }

    t = std::clamp(t, 0.0f, 1.0f);

    // 4. 값을 갱신합니다.
    float newValue = m_min + (m_max - m_min) * t;
    SetValue(newValue);
}
