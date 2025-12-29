#pragma once
#include <unordered_map>
#include <wrl/client.h>

#include "UIBase.h"
#include "UIImage.h"
#include "UIText.h"

#include "UIRenderer.h"

class UIButton : public UIBase
{
public:
    using ClickCallback = std::function<void()>;

    bool Initialize() override;
    //virtual void Update(float dt);
    //virtual void Render() = 0;
    //virtual void Shutdown();

    //virtual bool HitTest(const POINT& mousePos) const;

    void SetOnClick(ClickCallback cb);

    void Render() override;

    void SetText(const std::wstring& text = L"")
    {
        m_text->SetText(text);
    }

    void SetImage(const std::wstring& path, State state);
    void SetState(State s) override;

protected:
    void OnMouseEnter() override;
    void OnMouseLeave() override;
    void OnMouseMove(const POINT&) override;
    void OnMouseDown(const POINT&) override;
    void OnMouseUp(const POINT&) override;
    void OnClick() override;

private:
    void UpdateVisual();

    ClickCallback m_onClick;

    UIImage* m_background;
    UIText* m_text;

    std::unordered_map<State, ComPtr<ID2D1Bitmap>> m_stateImages;
};

struct Button
{
    UIButton* handle;
};