#include "pch.h"
#include "Live2DRenderer.h"

bool Live2DRenderer::Initialize(HWND hWnd, ID3D11Device* device, ID3D11DeviceContext* context, IDXGISwapChain* swapChain)
{
    ScreenSpaceRenderer::Initialize(hWnd, device, context, swapChain);

    return true;
}
