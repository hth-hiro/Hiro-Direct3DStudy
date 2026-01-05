#pragma once
#include "UIBase.h"
#include "UIImage.h"

enum class ImageType
{
    Track, Handle
};

class UISlider : public UIBase
{
public:
    using ValueChangedCallback = std::function<void(float)>;

    bool Initialize() override;
    void Render() override;
    void Update(float dt) override;

    void SetImage(const std::wstring& path, ImageType type);

    void SetRange(float min, float max);
    void SetValue(float v, bool notify = true);
    float GetValue() const { return m_value; }

    void SetOnValueChanged(ValueChangedCallback cb);

protected:
    void OnMouseDown(const POINT& pt) override;
    void OnMouseMove(const POINT& pt) override;
    void OnMouseUp(const POINT& pt) override;

private:
    void UpdateThumbByValue();
    void UpdateValueByMouse(const POINT& pt);

    // 슬라이더 바 / 손잡이
    UIImage* m_track = nullptr;
    UIImage* m_handle = nullptr;

    float m_min = 0.0f;
    float m_max = 1.0f;
    float m_value = 0.0f;
    bool m_dragging = false;

    std::wstring    m_trackPath = L"";
    std::wstring    m_thumbPath = L"";

    ValueChangedCallback m_onValueChanged = nullptr;
};

