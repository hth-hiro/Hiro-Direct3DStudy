#include "pch.h"
#include "UIText.h"
#include "UIRenderer.h"

UIText::UIText()
{
    //Initialize();
}

bool UIText::Initialize()
{
    auto* rt = UIRenderer::Get().GetRenderTarget();
    auto* dw = UIRenderer::Get().GetDWriteFactory();

    HRESULT hr;

    if (!rt || !dw)
        return false;

    m_fontFamily = L"Malgun Gothic";
    m_fontSize = 24.0f;

    hr = dw->CreateTextFormat(
        m_fontFamily.c_str(),
        nullptr,
        DWRITE_FONT_WEIGHT_NORMAL,
        DWRITE_FONT_STYLE_NORMAL,
        DWRITE_FONT_STRETCH_NORMAL,
        m_fontSize,
        L"ko-kr",
        m_textFormat.GetAddressOf()
    );
    if (FAILED(hr)) return false;

    hr = rt->CreateSolidColorBrush(
        D2D1::ColorF(D2D1::ColorF::White),
        m_textBrush.GetAddressOf()
    );
    if (FAILED(hr)) return false;

    LoadFontsFromDirectory(L"..\\Resource\\Fonts");

    return true;
}

void UIText::Shutdown()
{
    m_textFormats.clear();
    m_textBrush.Reset();
    m_textFormat.Reset();
}

void UIText::Render()
{
    /*auto* rt = UIRenderer::Get().GetRenderTarget();

    m_textBrush->SetColor(m_color);

    IDWriteTextFormat* format = GetTextFormat(m_fontSize, m_fontFamily.c_str());

    RECT wr = GetWorldBounds();
    D2D1_RECT_F r = D2D1::RectF((float)wr.left, (float)wr.top, (float)wr.right, (float)wr.bottom);

    rt->DrawTextW(
        m_text.c_str(),
        (UINT32)m_text.length(),
        format,
        r,
        m_textBrush.Get()
    );*/

    UpdateLayout();
    if (m_textLayout)
    {
        auto worldPos = GetWorldBounds();

        m_textBrush->SetColor(m_color);
        auto* rt = UIRenderer::Get().GetRenderTarget();

        rt->DrawTextLayout(
            D2D1::Point2F((float)worldPos.left, (float)worldPos.top),
            m_textLayout.Get(),
            m_textBrush.Get()
        );
    }
}

void UIText::SetText(const std::wstring& text)
{
    if (m_text == text) return;
    m_text = text;

    m_isLayoutDirty = true;
}

void UIText::SetRect(int x, int y, int width, int height)
{
    SetBounds({ x, y, x + width, y + height });

    //m_rect = D2D1::RectF(
    //    x, y,
    //    x + width, y + height);
}

void UIText::SetFont(const std::wstring& fontFamilyName)
{
    if (m_fontFamily == fontFamilyName);
    m_fontFamily = fontFamilyName;
    m_isLayoutDirty = true;
}

void UIText::SetFontSize(float fontSize)
{
    if (fabsf(m_fontSize - fontSize) < 0.001f) return;
    m_fontSize = fontSize;
    m_isLayoutDirty = true;
}

void UIText::SetColor(D2D1::ColorF color)
{
    if (m_color.r == color.r &&
        m_color.g == color.g &&
        m_color.b == color.b &&
        m_color.a == color.a)
        return;

    m_color = color;
}

void UIText::SetColor(Vector4 color)
{
    m_color.r = color.x;
    m_color.g = color.y;
    m_color.b = color.z;
    m_color.a = color.w;
}

IDWriteTextFormat* UIText::GetTextFormat(float fontSize, const WCHAR* fontFamilyName)
{
    TextFormatKey key{ fontFamilyName, fontSize };

    auto* dw = UIRenderer::Get().GetDWriteFactory();

    auto it = m_textFormats.find(key);
    if (it != m_textFormats.end())
        return it->second.Get();

    ComPtr<IDWriteTextFormat> format;

    HRESULT hr = dw->CreateTextFormat(
        fontFamilyName,
        nullptr,
        DWRITE_FONT_WEIGHT_NORMAL,
        DWRITE_FONT_STYLE_NORMAL,
        DWRITE_FONT_STRETCH_NORMAL,
        fontSize,
        L"ko-kr",
        format.GetAddressOf()
    );

    if (FAILED(hr)) return nullptr;

    m_textFormats.emplace(key, format);

    return format.Get();
}

bool UIText::LoadFontsFromDirectory(const std::wstring& directory)
{
    auto* dw = UIRenderer::Get().GetDWriteFactory();

    namespace fs = std::filesystem;

    for (const auto& entry : fs::directory_iterator(directory))
    {
        if (!entry.is_regular_file()) continue;

        auto ext = entry.path().extension().wstring();

        if (ext == L".ttf" || ext == L".ttc" || ext == L".otf")
        {
            AddFontResourceExW(entry.path().c_str(),
                FR_PRIVATE, nullptr);
        }
    }

    ComPtr<IDWriteFontCollection> collection;
    HRESULT hr = dw->GetSystemFontCollection(&collection, TRUE);

    return SUCCEEDED(hr);
}

void UIText::UpdateLayout()
{
    if (!m_isLayoutDirty) return;

    auto* dw = UIRenderer::Get().GetDWriteFactory();
    auto* format = GetTextFormat(m_fontSize, m_fontFamily.c_str());

    m_textLayout.Reset();

    if (m_text.empty())
    {
        m_isLayoutDirty = false;
        return;
    }

    float layoutW = max(Rect().m_size.x, 1.0f);
    float layoutH = max(Rect().m_size.y, 1.0f);

    HR_T(dw->CreateTextLayout(m_text.c_str(), (UINT32)m_text.length(),
        format,
        layoutW, layoutH,
        m_textLayout.GetAddressOf()));

    m_isLayoutDirty = false;
}
