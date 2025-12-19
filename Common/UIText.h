#pragma once
#include "UIBase.h"

#include <wrl/client.h>
#include <d3d11.h>
#include <dxgi1_2.h>
#include <d2d1_1.h>
#include <dwrite.h>
#include <string>

#include <DirectXMath.h>
#include <map>

#include <filesystem>

using Microsoft::WRL::ComPtr;
using namespace DirectX;

struct TextFormatKey
{
    std::wstring family;
    float size;

    bool operator<(const TextFormatKey& other) const
    {
        if (family != other.family)
            return family < other.family;

        return size < other.size;
    }
};

class UIText : public UIBase
{
public:
    UIText();
    ~UIText();

    bool Initialize() override;
    void Shutdown() override;
    //void Update(float dt) override;
    void Render() override;

public:
    void SetText(const std::wstring& text);
    void SetRect(int x, int y, int width, int height);
    void SetFont(const std::wstring& fontFamilyName = L"Malgun Gothic");
    void SetFontSize(float fontSize);
    void SetColor(D2D1::ColorF color);
    void SetColor(Vector4 color);
    void SetLayer(int layer);

private:
    // 기본 리소스
    ComPtr<IDWriteTextFormat>  m_textFormat;
    ComPtr<ID2D1SolidColorBrush> m_textBrush;
    ComPtr<IDWriteFontCollection> m_fontCollection;

    std::map<TextFormatKey, ComPtr<IDWriteTextFormat>> m_textFormats;

    IDWriteTextFormat* GetTextFormat(float fontSize, const WCHAR* fontFamilyName);
    bool LoadFontsFromDirectory(const std::wstring& directory);

private:
    std::wstring     m_text = L"";
    std::wstring     m_fontFamily = L"Malgun Gothic";
    float            m_fontSize = 24.0f;
    D2D1_RECT_F      m_rect;
    D2D1::ColorF     m_color = D2D1::ColorF(D2D1::ColorF::White);
};

