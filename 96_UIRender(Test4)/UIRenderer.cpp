#include "UIRenderer.h"
#include <cassert>

#pragma comment(lib, "d2d1.lib")
#pragma comment(lib, "dwrite.lib")

using Microsoft::WRL::ComPtr;

UIRenderer::UIRenderer()
{
}

void UIRenderer::ShutDown()
{
    m_textFormats.clear();
    m_textBrush.Reset();
    m_textFormat.Reset();
    m_d2dRenderTarget.Reset();
    m_dwriteFactory.Reset();
    m_d2dFactory.Reset();
}

bool UIRenderer::Initialize(HWND hWnd, ID3D11Device* device, ID3D11DeviceContext* context, IDXGISwapChain* swapChain)
{
    HRESULT hr;

    // Direct2D Factory
    hr = D2D1CreateFactory(
        D2D1_FACTORY_TYPE_SINGLE_THREADED,
        m_d2dFactory.GetAddressOf()
    );
    if (FAILED(hr)) return false;


    hr = DWriteCreateFactory(
        DWRITE_FACTORY_TYPE_SHARED,
        __uuidof(IDWriteFactory),
        reinterpret_cast<IUnknown**>(m_dwriteFactory.GetAddressOf())
    );
    if (FAILED(hr)) return false;


    ComPtr<IDXGISurface> surface;
    hr = swapChain->GetBuffer(
        0,
        __uuidof(IDXGISurface),
        reinterpret_cast<void**>(surface.GetAddressOf())
    );
    if (FAILED(hr)) return false;


    D2D1_RENDER_TARGET_PROPERTIES props =
        D2D1::RenderTargetProperties(
            D2D1_RENDER_TARGET_TYPE_DEFAULT,
            D2D1::PixelFormat(
                DXGI_FORMAT_UNKNOWN,
                D2D1_ALPHA_MODE_PREMULTIPLIED
            )
        );

    hr = m_d2dFactory->CreateDxgiSurfaceRenderTarget(
        surface.Get(),
        &props,
        m_d2dRenderTarget.GetAddressOf()
    );
    if (FAILED(hr)) return false;


    hr = m_dwriteFactory->CreateTextFormat(
        L"Malgun Gothic",
        nullptr,
        DWRITE_FONT_WEIGHT_NORMAL,
        DWRITE_FONT_STYLE_NORMAL,
        DWRITE_FONT_STRETCH_NORMAL,
        24.0f,
        L"ko-kr",
        m_textFormat.GetAddressOf()
    );
    if (FAILED(hr)) return false;


    hr = m_d2dRenderTarget->CreateSolidColorBrush(
        D2D1::ColorF(D2D1::ColorF::White),
        m_textBrush.GetAddressOf()
    );
    if (FAILED(hr)) return false;

    return true;
}

void UIRenderer::BeginFrame()
{
    if (!m_d2dRenderTarget) return;

    m_d2dRenderTarget->BeginDraw();
}

void UIRenderer::EndFrame()
{
    if (!m_d2dRenderTarget) return;

    m_d2dRenderTarget->EndDraw();
}

void UIRenderer::RenderText(const std::wstring& text, float x, float y, float fontSize, const XMFLOAT4& color)
{
    IDWriteTextFormat* format = GetTextFormat(fontSize);

    D2D1_RECT_F rect = D2D1::RectF(
        x, y,
        x + 1000.0f,
        y + 200.0f    //юс╫ц
    );

    m_textBrush->SetColor(D2D1::ColorF(color.x, color.y, color.z, color.w));

    m_d2dRenderTarget->DrawTextW(
        text.c_str(),
        (UINT32)text.length(),
        format,
        rect,
        m_textBrush.Get()
    );
}

IDWriteTextFormat* UIRenderer::GetTextFormat(float fontSize)
{
    auto it = m_textFormats.find(fontSize);
    if (it != m_textFormats.end())
        return it->second.Get();

    ComPtr<IDWriteTextFormat> format;

    HRESULT hr = m_dwriteFactory->CreateTextFormat(
        L"Malgun Gothic",
        nullptr,
        DWRITE_FONT_WEIGHT_NORMAL,
        DWRITE_FONT_STYLE_NORMAL,
        DWRITE_FONT_STRETCH_NORMAL,
        fontSize,
        L"ko-kr",
        format.GetAddressOf()
    );

    if (FAILED(hr)) return nullptr;

    m_textFormats.emplace(fontSize, format);

    return format.Get();
}
