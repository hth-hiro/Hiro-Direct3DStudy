#include "pch.h"
#include "UIRenderer.h"
#include <cassert>

#pragma comment(lib, "d2d1.lib")
#pragma comment(lib, "dwrite.lib")

UIRenderer::UIRenderer()
{
}

UIRenderer::~UIRenderer()
{
    //ShutDown();
}

bool UIRenderer::Initialize(HWND hWnd, ID3D11Device* device, ID3D11DeviceContext* context, IDXGISwapChain* swapChain)
{
    ScreenSpaceRenderer::Initialize(hWnd, device, context, swapChain);

    HRESULT hr;

    // UIText =============================================================
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

    // UIImage ========================================================
    hr = CoCreateInstance(
        CLSID_WICImagingFactory,
        nullptr,
        CLSCTX_INPROC_SERVER,
        IID_PPV_ARGS(m_wicFactory.GetAddressOf())
    );
    if (FAILED(hr)) return false;

    return true;
}

void UIRenderer::ShutDown()
{
    if (m_isShutdown) return;
    m_isShutdown = true;

    for (auto& kv : m_bitmaps)
        kv.second.Detach();
    m_bitmaps.clear();

    m_solidBrush.Reset();
    m_wicFactory.Reset();
    m_d2dRenderTarget.Reset();
    m_dwriteFactory.Reset();
    m_d2dFactory.Reset();

    //m_solidBrush.Detach();
    //m_wicFactory.Detach();
    //m_d2dRenderTarget.Detach();
    //m_dwriteFactory.Detach();
    //m_d2dFactory.Detach();
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

ID2D1RenderTarget* UIRenderer::GetRenderTarget()
{
    return m_d2dRenderTarget.Get();
}

IDWriteFactory* UIRenderer::GetDWriteFactory()
{
    return m_dwriteFactory.Get();
}

void UIRenderer::DrawImage(ID2D1Bitmap* bmp, const RECT& dst)
{
    auto* rt = m_d2dRenderTarget.Get();
    if (!rt || !bmp) return;

    D2D1_RECT_F r = D2D1::RectF(
        (float)dst.left, (float)dst.top,
        (float)dst.right, (float)dst.bottom);

    rt->DrawBitmap(
        bmp,
        r,
        1.0f,
        D2D1_BITMAP_INTERPOLATION_MODE_LINEAR
    );
}

// 경로로 비트맵 로드하기
ComPtr<ID2D1Bitmap> UIRenderer::LoadBitmapFromFile(const std::wstring& path)
{
    ComPtr<ID2D1Bitmap> out;

    if (!m_wicFactory || !m_d2dRenderTarget) return out;

    ComPtr<IWICBitmapDecoder> decoder;
    HRESULT hr = m_wicFactory->CreateDecoderFromFilename(
        path.c_str(), nullptr, GENERIC_READ, WICDecodeMetadataCacheOnLoad, decoder.ReleaseAndGetAddressOf());
    if (FAILED(hr)) return out;

    ComPtr<IWICBitmapFrameDecode> frame;
    hr = decoder->GetFrame(0, frame.ReleaseAndGetAddressOf());
    if (FAILED(hr)) return out;

    ComPtr<IWICFormatConverter> converter;
    hr = m_wicFactory->CreateFormatConverter(converter.ReleaseAndGetAddressOf());
    if (FAILED(hr)) return out;

    hr = converter->Initialize(
        frame.Get(),
        GUID_WICPixelFormat32bppPBGRA,
        WICBitmapDitherTypeNone,
        nullptr,
        0.0,
        WICBitmapPaletteTypeMedianCut
    );
    if (FAILED(hr)) return out;

    hr = m_d2dRenderTarget->CreateBitmapFromWicBitmap(
        converter.Get(),
        nullptr,
        out.ReleaseAndGetAddressOf()
    );

    return SUCCEEDED(hr) ? out : ComPtr<ID2D1Bitmap>{};
}

void UIRenderer::FillRect(const RECT& rc, const D2D1_COLOR_F& color)
{
    auto* rt = m_d2dRenderTarget.Get();
    if (!rt) return;

    if (!m_solidBrush)
    {
        rt->CreateSolidColorBrush(D2D1::ColorF(1, 1, 1, 1), m_solidBrush.GetAddressOf());
    }

    m_solidBrush->SetColor(color);

    D2D1_RECT_F r = D2D1::RectF(
        (float)rc.left, (float)rc.top,
        (float)rc.right, (float)rc.bottom
    );

    rt->FillRectangle(r, m_solidBrush.Get());
}
