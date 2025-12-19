#include "pch.h"
#include "UIImage.h"
#include "UIRenderer.h"

UIImage::UIImage()
{
    Initialize();
}

UIImage::~UIImage()
{
    Shutdown();
}

bool UIImage::Initialize()
{
    return true;
}

void UIImage::Shutdown()
{
}

void UIImage::Render()
{
}
