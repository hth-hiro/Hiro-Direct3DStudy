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
    ShutDown();
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

    return true;
}

void UIRenderer::ShutDown()
{
    m_d2dRenderTarget.Reset();
    m_dwriteFactory.Reset();
    m_d2dFactory.Reset();
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