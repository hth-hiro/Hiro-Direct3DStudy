#include "framework.h"
#include "02_Mesh.h"

#include "TutorialApp.h"

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	TutorialApp App(hInstance);

	App.Initialize(1024, 768);

	return App.Run();
}
