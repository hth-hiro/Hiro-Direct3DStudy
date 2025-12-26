#pragma once
#include "UIBase.h"
#include "UIImage.h"
#include "UIText.h"

class UIButton : public UIBase
{
public:
    using ClickCallback = std::function<void()>;

    void SetOnClick(ClickCallback cb);

    void Render() override;

    void SetText(const std::wstring& text)
    {
        m_text->SetText(text);
    }

protected:
    void OnMouseEnter() override;
    void OnMouseLeave() override;
    void OnMouseMove(const POINT&) override;
    void OnMouseDown(const POINT&) override;
    void OnMouseUp(const POINT&) override;
    void OnClick() override;

private:
    ClickCallback m_onClick;

    UIImage* m_background;
    UIText* m_text;
};

