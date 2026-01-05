#pragma once
#include "UIBase.h"
#include <DirectXMath.h>
#include <Windows.h>
#include <string>
using namespace DirectX;

//inline std::string WStringToUtf8(const std::wstring& w)
//{
//    if (w.empty()) return {};
//
//    int size = WideCharToMultiByte(
//        CP_UTF8, 0,
//        w.data(), (int)w.size(),
//        nullptr, 0, nullptr, nullptr);
//
//    std::string result(size, 0);
//    WideCharToMultiByte(
//        CP_UTF8, 0,
//        w.data(), (int)w.size(),
//        result.data(), size, nullptr, nullptr);
//
//    return result;
//}
//
//inline std::wstring Utf8ToWString(const std::string& s)
//{
//    if (s.empty()) return {};
//
//    int size = MultiByteToWideChar(
//        CP_UTF8, 0,
//        s.data(), (int)s.size(),
//        nullptr, 0);
//
//    std::wstring result(size, 0);
//    MultiByteToWideChar(
//        CP_UTF8, 0,
//        s.data(), (int)s.size(),
//        result.data(), size);
//
//    return result;
//}

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
    ~UIText() = default;

    bool Initialize() override;
    void Shutdown() override;
    //void Update(float dt) override;
    void Render() override;

public:
    void SetText(const std::wstring& text);
    //void SetText(const std::string& text);
    void SetFont(const std::wstring& fontFamilyName = L"Malgun Gothic");
    void SetFontSize(float fontSize);
    void SetColor(D2D1::ColorF color);
    void SetColor(Vector4 color);
    void SetRect(int x, int y, int width, int height);

    IDWriteTextLayout* GetLayout() { return m_textLayout.Get(); }

private:
    // 기본 리소스
    ComPtr<IDWriteTextFormat>       m_textFormat;
    ComPtr<ID2D1SolidColorBrush>    m_textBrush;
    ComPtr<IDWriteFontCollection>   m_fontCollection;
    ComPtr<IDWriteTextLayout>       m_textLayout;

    std::map<TextFormatKey, ComPtr<IDWriteTextFormat>> m_textFormats;

    IDWriteTextFormat* GetTextFormat(float fontSize, const WCHAR* fontFamilyName);
    bool LoadFontsFromDirectory(const std::wstring& directory);
    void UpdateLayout();

private:
    std::wstring     m_text = L"";
    std::wstring     m_fontFamily = L"Malgun Gothic";
    float            m_fontSize = 24.0f;
    bool             m_isLayoutDirty = true;
    D2D1_RECT_F      m_rect;
    D2D1::ColorF     m_color = D2D1::ColorF(D2D1::ColorF::White);
};

