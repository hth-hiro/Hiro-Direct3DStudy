#pragma once
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
#include <unordered_map>

using namespace std;
using Microsoft::WRL::ComPtr;
using namespace DirectX;

class UIRenderer
{
public:
    UIRenderer();
    
    void ShutDown();

    bool Initialize(HWND hWnd, ID3D11Device* device, ID3D11DeviceContext* context, IDXGISwapChain* swapChain);

    //void Resize(int screenWidth, int screenHeight);

    void BeginFrame();
    void EndFrame();

    void RenderText(
        const std::wstring& text,
        float x,
        float y,
        float fontSize = 24.0f,
        const XMFLOAT4& color = {1, 1, 1, 1}
    );

private:
    ComPtr<ID2D1Factory>       m_d2dFactory;
    ComPtr<ID2D1RenderTarget>  m_d2dRenderTarget;
    ComPtr<IDWriteFactory>     m_dwriteFactory;

    // 기본 리소스
    ComPtr<IDWriteTextFormat>  m_textFormat;
    ComPtr<ID2D1SolidColorBrush> m_textBrush;

    std::unordered_map<float, ComPtr<IDWriteTextFormat>> m_textFormats;

    IDWriteTextFormat* GetTextFormat(float fontSize);

    float                   m_screenWidth = 0.0f;
    float                   m_screenHeight = 0.0f;
};

