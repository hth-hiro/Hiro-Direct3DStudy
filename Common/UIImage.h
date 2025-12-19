#pragma once
#include "UIBase.h"

class UIImage : public UIBase
{
public:
    UIImage();
    ~UIImage();

    bool Initialize() override;
    void Shutdown() override;
    //void Update(float dt) override;
    void Render() override;

public:
    // TODO: 여기에 이미지 띄울 함수 추가

private:
    // TODO: 이미지를 띄울 리소스
};

