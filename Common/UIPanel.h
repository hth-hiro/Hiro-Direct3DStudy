#pragma once
#include "UIBase.h"

class UIPanel : public UIBase
{
public:
    UIPanel()
    {
        // 패널은 입력을 받지 않음.
        SetEnabled(false);
    }

    void SetDrawBackground(bool draw) { m_drawBackground = draw; }
    void SetBackgroundColor(const D2D1_COLOR_F& color)
    {
        m_drawBackground = true;
        m_bgColor = color;
    }

    void Render() override;

private:
    bool m_drawBackground = false;
    bool m_blockInput = false;
    
    D2D1_COLOR_F m_bgColor = D2D1::ColorF(0.2f, 0.2f, 0.2f, 0.8f);
};

