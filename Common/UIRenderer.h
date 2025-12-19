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

private:
    ComPtr<ID2D1RenderTarget>  m_d2dRenderTarget;
    ComPtr<ID2D1Factory>       m_d2dFactory;
    ComPtr<IDWriteFactory>     m_dwriteFactory;
};