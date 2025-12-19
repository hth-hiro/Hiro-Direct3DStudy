#pragma once
#include <iostream> 
#include "Helper.h"

class UIBase
{
public:
    UIBase();
    virtual ~UIBase();

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

