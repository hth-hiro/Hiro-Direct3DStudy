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
#include <map>

using namespace std;
using Microsoft::WRL::ComPtr;
using namespace DirectX;

class ScreenSpaceRenderer
{
protected:
    ID3D11Device*            m_device = nullptr;
    ID3D11DeviceContext*     m_context = nullptr;
    IDXGISwapChain*          m_swapChain = nullptr;

public:
    virtual ~ScreenSpaceRenderer() = default;

    virtual bool Initialize(HWND hWnd, ID3D11Device* dev, ID3D11DeviceContext* context, IDXGISwapChain* swapChain)
    {
        m_device = dev;
        m_context = context;
        m_swapChain = swapChain;
        return true;
    }
};

