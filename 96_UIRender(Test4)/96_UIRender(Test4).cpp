// 96_UIRender(Test4).cpp : 애플리케이션에 대한 진입점을 정의합니다.
//

#include "framework.h"
#include "96_UIRender(Test4).h"

#include "TutorialApp.h"
#include <iostream>

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR    lpCmdLine,
    _In_ int       nCmdShow)
{
    TutorialApp App(hInstance);

    App.Initialize(1920, 1080);

    App.Run();

    return 0;
}