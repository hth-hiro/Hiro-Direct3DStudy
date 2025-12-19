#pragma once
#include <iostream> 
#include "Helper.h"

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

    void AddChild(std::unique_ptr<UIBase> child);

protected:
    RECT m_bounds;
    bool m_active = true;
    int m_zOrder = 0;
};

