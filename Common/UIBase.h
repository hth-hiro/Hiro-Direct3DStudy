#pragma once
#include <iostream> 
#include "Helper.h"

#include <wrl/client.h>
#include <d3d11.h>
#include <dxgi1_2.h>
#include <d2d1_1.h>
#include <dwrite.h>
#include <string>

#include <DirectXMath.h>
#include <map>

#include <filesystem>

using Microsoft::WRL::ComPtr;
using namespace DirectX;

// UIBase는 UI의 부모입니다.
// 모든 UI들을 UIBase가 보관하고, UIManager가 UIBase를 관리합니다.
class UIBase
{
public:
    virtual ~UIBase() = default;

    virtual bool Initialize() { return true; }
    virtual void Update(float dt) {}
    virtual void Render() = 0;
    virtual void Shutdown() {}

    virtual bool HitTest(const POINT& mousePos);

    void SetActive(bool active);
    bool IsActive() const;

    int GetZOrder() const { return m_zOrder; }
    void SetZOrder(int z) { m_zOrder = z; }

    void AddChild(std::unique_ptr<UIBase> child);

protected:
    RECT m_bounds;
    bool m_active = true;
    int m_zOrder = 0;
};

