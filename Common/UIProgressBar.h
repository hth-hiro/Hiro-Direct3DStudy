#pragma once
#include "UIBase.h"
#include "UIImage.h"

enum class ProgressType
{
    Linear, Radial
};

class UIProgressBar : public UIBase
{
public:
    enum class ImageType { Frame, Fill };

    bool Initialize() override;
    void Update(float dt) override;
    void Render() override;

    void SetProgress(float ratio);

    void SetImage(const std::wstring& path, ImageType type);

    void SetFillColor(const Vector4& color);

private:
    UIImage* m_background = nullptr;
    UIImage* m_fill = nullptr;

    std::wstring m_backgroundPath = L"../Resource/UI/ProgressBar/Background.png";
    std::wstring m_fillPath = L"../Resource/UI/ProgressBar/Fill.png";

    Vector4 m_color = { 1, 1, 1, 1 };

    float m_progress = 0.0f;
};

