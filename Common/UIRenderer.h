#pragma once
#include "Utility/Singleton.h"
#include "ScreenSpaceRenderer.h"

#include <wrl/client.h>
#include <d3d11.h>
#include <dxgi1_2.h>
#include <d2d1_1.h>
#include <dwrite.h>
#include <string>

#include <DirectXMath.h>
#include <memory>
#include <directxtk/SpriteBatch.h>
#include <directxtk/SpriteFont.h>

#include "../Common/Helper.h"
#include <map>
#include <wincodec.h>
#pragma comment(lib, "windowscodecs.lib")

using namespace std;
using Microsoft::WRL::ComPtr;
using namespace DirectX;

class UIRenderer : public Singleton<UIRenderer>, public ScreenSpaceRenderer
{
public:
    UIRenderer();
    ~UIRenderer();

    bool Initialize(HWND hWnd, ID3D11Device* device, ID3D11DeviceContext* context, IDXGISwapChain* swapChain);
    void ShutDown();

    void BeginFrame();
    void EndFrame();

    ID2D1RenderTarget* GetRenderTarget();
    IDWriteFactory* GetDWriteFactory();

    ComPtr<ID2D1Bitmap> GetBitmap(const std::wstring& path)
    {
        auto it = m_bitmaps.find(path);
        if (it != m_bitmaps.end())
            return it->second;

        auto bmp = LoadBitmapFromFile(path);
        if (bmp) m_bitmaps.emplace(path, bmp);

        return bmp;
    }

    // UIImage가 호출
    void DrawImage(ID2D1Bitmap* bmp, const RECT& dst);
    ComPtr<ID2D1Bitmap> LoadBitmapFromFile(const std::wstring& path);

    // UIPanel이 호출
    void FillRect(const RECT& rc, const D2D1_COLOR_F& color);

private:
    ComPtr<ID2D1RenderTarget>  m_d2dRenderTarget;
    ComPtr<ID2D1Factory>       m_d2dFactory;
    ComPtr<IDWriteFactory>     m_dwriteFactory;

    ComPtr<IWICImagingFactory> m_wicFactory;

    ComPtr<ID2D1SolidColorBrush> m_solidBrush;

    std::unordered_map<std::wstring, ComPtr<ID2D1Bitmap>> m_bitmaps;

    bool m_isShutdown = false;
};