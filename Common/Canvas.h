#pragma once
#include "UIBase.h"
#include "RectTransform.h"

// Canvas는 UI의 루트 노드입니다.
class Canvas : public UIBase
{
public:
    Canvas();
    Canvas(float w, float h);

    void Render() override
    {
        RenderChildren();
    }
};

