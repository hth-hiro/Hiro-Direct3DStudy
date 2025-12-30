#pragma once
#include "UIBase.h"

// Canvas는 UI의 루트 노드입니다.
class Canvas : public UIBase
{
public:
    Canvas() = default;

    bool active = true;

    void Render() override
    {
        RenderChildren();
    }


};

