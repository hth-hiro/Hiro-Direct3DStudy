#include "framework.h"
#include "D3D Class.h"

#include "TutorialApp.h"

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    TutorialApp App(hInstance);

    if (!App.Initialize(1024, 768))
        return -1;

    App.Run();
}