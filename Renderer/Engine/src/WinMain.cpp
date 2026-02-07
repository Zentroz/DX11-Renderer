#include"Window/Window.h"
#include"Engine.h"

#include<stdexcept>
#include <cstdio>

void CreateConsole()
{
	AllocConsole();

	FILE* fp;
	freopen_s(&fp, "CONOUT$", "w", stdout);
	freopen_s(&fp, "CONOUT$", "w", stderr);
	freopen_s(&fp, "CONIN$", "r", stdin);

	SetConsoleTitle(L"DX11 Debug Console");
}

int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ PWSTR pCmdLine, _In_ int nCmdShow) {
	CreateConsole();

	int width = 1920;
	int height = 1080;

	Window window{};
	Engine engine{};

	window.Init({.width = width, .height = height});
	window.SetEngine(&engine);

	EngineInitData engineInit{
		.hWnd = window.GetHWND()
	};
	window.GetClientSize(engineInit.width, engineInit.height);
	engine.Init(engineInit);

	while (!window.ShouldClose()) {
		window.Process();
		engine.Run();
	}

	engine.CleanUp();
	window.CleanUp();

	return 0;
}