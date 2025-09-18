// 04_Texture.cpp : 애플리케이션에 대한 진입점을 정의합니다.
//

#include "framework.h"
#include "04_Texture.h"

#include "TutorialApp.h"
//#include <iostream>
//
//void RedirectIOToConsole()
//{
//    AllocConsole(); // 콘솔 창 생성
//
//    FILE* fp;
//    freopen_s(&fp, "CONOUT$", "w", stdout); // 표준 출력 리디렉션
//    freopen_s(&fp, "CONOUT$", "w", stderr); // 표준 에러 리디렉션
//    freopen_s(&fp, "CONIN$", "r", stdin);   // 표준 입력 리디렉션
//
//    std::ios::sync_with_stdio(); // C++ 입출력 동기화
//}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
    //RedirectIOToConsole();

	TutorialApp App(hInstance);

	App.Initialize(1024, 768);

	App.Run();

    //FreeConsole();

    return 0;
}