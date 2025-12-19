#pragma once
#include "Helper.h"

class UIBase
{
public:
    virtual ~UIBase() = default;

    virtual void Update(float dt) {}
    virtual void Render() = 0;

    virtual bool HitTest(const POINT& mousePos);

    void SetActive(bool active);
    bool IsActive() const;

protected:
    RECT m_bounds;
    bool m_active = true;
    int m_zOrder = 0;
};

