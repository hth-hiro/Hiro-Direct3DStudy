#pragma once
#include "Utility/Singleton.h"
#include "ScreenSpaceRenderer.h"

class Live2DRenderer : public Singleton<Live2DRenderer>, public ScreenSpaceRenderer
{
public:
    bool Initialize(HWND hWnd, ID3D11Device* device, ID3D11DeviceContext* context, IDXGISwapChain* swapChain);
};