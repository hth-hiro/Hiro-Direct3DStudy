#pragma once
#include "UIBase.h"
#include "UIImage.h"
#include "UIText.h"

class UIToggle : public UIBase
{
public:
    enum class ImageType
    {
        Frame, Mark
    };

    using OnToggleChanged = std::function<void(bool)>;

    bool Initialize() override;
    void Update(float dt) override;
    void Render() override;

    void SetIsOn(bool isOn, bool notify = true);
    bool IsChecked() const { return m_isOn; }

    // 이미지 설정
    void SetImage(const std::wstring& path, ImageType type);

    void SetOnCheckedChanged(OnToggleChanged cb);

protected:
    void OnMouseDown(const POINT& pt) override;

private:
    bool m_isOn = false;

    UIImage* m_background = nullptr;
    UIImage* m_checkmark = nullptr;

    // 선택
    //UIText* m_label = nullptr;

    std::wstring m_backgroundPath = L"../Resource/UI/Toggle/Background.png";
    std::wstring m_checkmarkPath = L"../Resource/UI/Toggle/Checkmark.png";

    OnToggleChanged m_onValueChanged = nullptr;
};

