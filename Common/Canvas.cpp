#include "pch.h"
#include "Canvas.h"

Canvas::Canvas()
{
}

Canvas::Canvas(float w, float h)
{
    Rect().SetPivot(0.0f, 0.0f);
    Rect().SetAnchoredPosition(0, 0);
    Rect().SetSize(w, h);
}

void Canvas::Render()
{
    RenderChildren();
}