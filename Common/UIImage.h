#pragma once
#include "UIBase.h"
#include "Define.h"

class UIImage : public UIBase
{
public:
    UIImage();
    ~UIImage();

    bool Initialize() override;
    void Shutdown() override;
    //void Update(float dt) override;
    void Render() override;

    void OnMouseDown(const POINT& p) override;
    void OnMouseMove(const POINT& p) override;
    void OnMouseUp(const POINT& p) override;

public:
    //void SetTexture(ID3D11ShaderResourceView* tex);

    bool SetImage(const std::wstring& path);
    void SetBitmap(ID2D1Bitmap* bmp) { m_bitmap = bmp; }

    //void SetTintColor(const Vector4& color);

    bool m_dragging = false;
    POINT m_dragOffset{ 0,0 };

private:
    // TODO: 이미지를 띄울 리소스
    //ComPtr<ID3D11ShaderResourceView> m_Texture;
    ComPtr<ID2D1Bitmap> m_bitmap;
};

