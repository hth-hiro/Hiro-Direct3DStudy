#include "pch.h"
#include "RectTransform.h"

void RectTransform::SetSize(float w, float h)
{
    Vector2 ns{ Max(0.0f, w), Max(0.0f, h) };
    if (m_size == ns) return;
    m_size = ns;
    MarkDirty();
}

void RectTransform::SetPivot(float x, float y)
{
    Vector2 pv{ std::clamp(x, 0.0f, 1.0f), std::clamp(y, 0.0f, 1.0f) };
    if (m_pivot == pv) return;
    m_pivot = pv;
    MarkDirty();
}

void RectTransform::SetAnchoredPosition(float x, float y)
{
    Vector2 np{ x, y };
    if (m_anchoredPosition == np) return;
    m_anchoredPosition = np;
    MarkDirty();
}
