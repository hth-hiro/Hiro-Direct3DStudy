#pragma once
#include "UIBase.h"
#include "UIText.h"
#include "UIImage.h"
#include "UIButton.h"
//#include "UISlider.h"
//#include "UICheckbox.h"
//#include "UIRadiobox.h"

class UIPanel : public UIBase
{
public:
    UIPanel()
    {
        // 패널은 입력을 받지 않음. (받게 하기)
        //SetEnabled(false);
    }

    void SetDrawBackground(bool draw) { m_drawBackground = draw; }
    void SetBackgroundColor(const D2D1_COLOR_F& color)
    {
        m_drawBackground = true;
        m_bgColor = color;
    }

    void Render() override;

    void SetRect(int x, int y, int width, int height);

    bool BlocksInput() const override { return true; }

private:
    bool m_drawBackground = false;
    
    D2D1_COLOR_F m_bgColor = D2D1::ColorF(0.2f, 0.2f, 0.2f, 0.8f);
};